
#include "WebTerrainRenderer.h"
#include "../../../ecs/components/renderable/TerrainTileRenderable.h"
#include "../../../ecs/components/lighting/Lights.h"

#include "shaders/sources/WebTestShader.h"

#include "../../../core/Application.h"

#include "../../../core/Timing.h"

#include "../../../Common.h"

namespace pk
{
	namespace web
	{

		static std::string s_vertexSource = R"(
                precision mediump float;
                
                attribute vec3 position;
                attribute vec2 uv;
                
				uniform mat4 projectionMatrix;
				uniform mat4 viewMatrix;
			
				varying vec2 var_uv;
				
				varying vec3 var_normal;
				varying vec3 var_fragPos;


                void main()
                {	
					gl_Position = projectionMatrix * viewMatrix * mat4(1.0) * vec4(position, 1.0);
					var_uv = uv;
					var_normal = vec3(0,1.0,0);
					var_fragPos = position;
				}
            )";

		static std::string s_fragmentSource = R"(
                precision mediump float;
                
				varying vec2 var_uv;
				varying vec3 var_normal;
				varying vec3 var_fragPos;
				

				uniform sampler2D tex_blendmap;

				uniform sampler2D tex_channel_black;
				uniform sampler2D tex_channel_red;
				uniform sampler2D tex_channel_green;
				uniform sampler2D tex_channel_blue;
				uniform sampler2D tex_channel_alpha;
				
				uniform vec3 dirLight_dir;
				
                void main()
                {
					vec3 lightDir = normalize(dirLight_dir);
					float diffFactor = clamp(dot(-lightDir, normalize(var_normal)), 0.0, 1.0);

					vec4 blendmapColor = texture2D(tex_blendmap, var_uv);
					vec2 tiledUvs = var_uv * (15.0 * 2.0 + 1.0);
					float amount_black = 1.0 - (blendmapColor.r + blendmapColor.g + blendmapColor.b + blendmapColor.a);
					
					vec4 tex_channel_black	=	texture2D(tex_channel_black, tiledUvs)  * amount_black;
					vec4 tex_channel_red	=	texture2D(tex_channel_red,	 tiledUvs)	* blendmapColor.r;
					vec4 tex_channel_green	=	texture2D(tex_channel_green, tiledUvs)	* blendmapColor.g;
					vec4 tex_channel_blue	=	texture2D(tex_channel_blue,	 tiledUvs)	* blendmapColor.b;
					vec4 tex_channel_alpha	=	texture2D(tex_channel_alpha, tiledUvs)	* blendmapColor.a;
					
					vec4 totalTextureColor = tex_channel_black + tex_channel_red + tex_channel_green + tex_channel_blue + tex_channel_alpha;

					gl_FragColor = totalTextureColor; // * diffFactor; 
				}
            )";
		
		WebTerrainRenderer::WebTerrainRenderer() : 
			_shader(s_vertexSource, s_fragmentSource)
		{
			_vertexAttribLocation_pos = _shader.getAttribLocation("position");
			_vertexAttribLocation_uv  = _shader.getAttribLocation("uv");
			
			_uniformLocation_projMat = _shader.getUniformLocation("projectionMatrix");
			_uniformLocation_viewMat = _shader.getUniformLocation("viewMatrix");
			
			_uniformLocation_texSampler_blendmap = _shader.getUniformLocation("tex_blendmap");
			_uniformLocation_texSampler_black	 = _shader.getUniformLocation("tex_channel_black");
			_uniformLocation_texSampler_red		 = _shader.getUniformLocation("tex_channel_red");
			_uniformLocation_texSampler_green	 = _shader.getUniformLocation("tex_channel_green");
			_uniformLocation_texSampler_blue	 = _shader.getUniformLocation("tex_channel_blue");
			_uniformLocation_texSampler_alpha	 = _shader.getUniformLocation("tex_channel_alpha");

			_uniformLocation_dirLight_dir = _shader.getUniformLocation("dirLight_dir");
			//_uniformLocation_dirLight_color = _shader.getUniformLocation("dirLight_color");

			
			// Load tex atlas
			_textureAtlas = new WebTexture(
				"assets/Terrain.png",
				{
					TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_NEAR,
					TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
				}
			);

			const int maxBatchInstanceCount = 512;
			const int batchInstanceDataLen = 5 * 4; // (pos:vec3, uv:vec2 --> 5 floats)
			allocateBatches(4, maxBatchInstanceCount * batchInstanceDataLen, batchInstanceDataLen);
		}
		
		WebTerrainRenderer::~WebTerrainRenderer()
		{
			for (BatchData& batch : _batches)
			{
				batch.destroy();
			}

			delete _textureAtlas;
		}



		// submit renderable component for rendering (batch preparing, before rendering)
		void WebTerrainRenderer::submit(const Component* const renderableComponent, const mat4& transformation)
		{
			// <#M_DANGER>
			const TerrainTileRenderable* const  renderable = (const TerrainTileRenderable* const)(renderableComponent);
			
			const float xPos = (float)renderable->worldX;
			const float zPos = (float)renderable->worldZ;
			
			const float scale = renderable->scale;

			float height_tl = renderable->vertexHeights[0];
			float height_tr = renderable->vertexHeights[1];
			float height_bl = renderable->vertexHeights[2];
			float height_br = renderable->vertexHeights[3];

			const vec2& texOffset = renderable->textureOffset;
			
			// We need to negate a single pixel on some uvs, otherwise ugly artifacts on tiles' borders
			const float pixelUvSpace = (1.0f / _texAtlasWidth);
			const float texAtlasRows = 8.0f;
			
			const float x_uvSpace = (float)renderable->gridX / (float)TerrainTileRenderable::s_gridWidth;
			const float y_uvSpace = (float)renderable->gridY / (float)TerrainTileRenderable::s_gridWidth;
			const float uvScaleMod = 1.0f / TerrainTileRenderable::s_gridWidth;

			// blendmap uvs
			vec2 uv_v0(x_uvSpace, y_uvSpace);
			vec2 uv_v1(x_uvSpace, y_uvSpace + uvScaleMod);
			vec2 uv_v2(x_uvSpace + uvScaleMod, y_uvSpace + uvScaleMod);
			vec2 uv_v3(x_uvSpace + uvScaleMod, y_uvSpace);

			//vec2 uv_v0(0, 1);
			//vec2 uv_v1(0, 0);
			//vec2 uv_v2(1, 0);
			//vec2 uv_v3(1, 1);

			//vec2 uv_texAtlas_v0 = (vec2(0, 1) + texOffset) / texAtlasRows;
			//vec2 uv_texAtlas_v1 = (vec2(0, 0) + texOffset) / texAtlasRows;
			//vec2 uv_texAtlas_v2 = (vec2(1, 0) + texOffset) / texAtlasRows;
			//vec2 uv_texAtlas_v3 = (vec2(1, 1) + texOffset) / texAtlasRows;

			const int batchIdentifier = 1; // atm everyone will go into the same batch, FOR TESTING PURPOSES!
			
			//float bleed = scale * 0.25f; // "one eighth of the scale" same as div by 8


			std::vector<float> dataToSubmit{ 
				xPos,				height_tl,		zPos,				uv_v0.x, uv_v0.y,
				xPos,				height_bl,		zPos + scale,		uv_v1.x, uv_v1.y,
				xPos + scale,		height_br,		zPos + scale,		uv_v2.x, uv_v2.y,
				xPos + scale,		height_tr,		zPos,				uv_v3.x, uv_v3.y
			};
			
			

			// Find suitable batch
			for (int i = 0; i < _batches.size(); ++i)
			{
				BatchData& batch = _batches[i];
				if (batch.isFull)
				{
					//Debug::log("Batch was full!");
					continue;
				}

				if (batch.isFree)
				{
					batch.occupy(batchIdentifier);
					batch.insertInstanceData(0, dataToSubmit);
					batch.addNewInstance();
					return;
				}
				else if (batch.ID == batchIdentifier)
				{
					batch.insertInstanceData(0, dataToSubmit);
					batch.addNewInstance();
					return;
				}
			}
		}


		void WebTerrainRenderer::render(const Camera& cam)
		{
			const mat4& projectionMatrix = cam.getProjMat3D();

			Scene* scene = Application::get()->accessCurrentScene();

			// Find scene's active camera's transform
			// This quite disgusting, just atm for testing purposes..
			const Transform* camTransform = (Transform*)scene->getComponent(cam.getEntity(), ComponentType::PK_TRANSFORM);
			mat4 viewMat = camTransform->getTransformationMatrix();
			viewMat.inverse(); // omg this quite heavy operation ...

			// Find scene's "sun(dir light)"
			DirectionalLight* dirLight = (DirectionalLight*)scene->getComponent(ComponentType::PK_LIGHT_DIRECTIONAL);
			
			glUseProgram(_shader.getProgramID());
			
			// all common uniforms..
			_shader.setUniform(_uniformLocation_projMat, projectionMatrix);
			_shader.setUniform(_uniformLocation_viewMat, viewMat);

			// Light
			if (dirLight)
			{
				// *light color not used atm
				//_shader.setUniform(_uniformLocation_dirLight_color, dirLight->color);
				_shader.setUniform(_uniformLocation_dirLight_dir, dirLight->direction);
			}

			

			
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			

			for (BatchData& batch : _batches)
			{
				if (batch.isFree)
					continue;

				
				WebVertexBuffer* vb = (WebVertexBuffer*)batch.vertexBuffers[0];
				WebIndexBuffer* ib = (WebIndexBuffer*)batch.indexBuffer;
				
				// hardcoded just as temp..
				const GLsizei instanceIndexCount = 6;

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getID());

				int stride = sizeof(float) * 5;
				glBindBuffer(GL_ARRAY_BUFFER, vb->getID());

				// position vertex attrib
				glEnableVertexAttribArray(_vertexAttribLocation_pos);
				glVertexAttribPointer(_vertexAttribLocation_pos, 3, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, stride, 0);
				
				// uv coord vertex attrib
				glEnableVertexAttribArray(_vertexAttribLocation_uv);
				glVertexAttribPointer(_vertexAttribLocation_uv, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
				

				// Bind textures
				// <#M_DANGER> atm these may be nullptr -> make sure they never can be in the future!
				const WebTexture* tex_blendmap = (const WebTexture*)TerrainTileRenderable::s_blendmapTexture;
				const WebTexture* tex0 = (const WebTexture*)TerrainTileRenderable::s_channelTexture0;
				const WebTexture* tex1 = (const WebTexture*)TerrainTileRenderable::s_channelTexture1;
				const WebTexture* tex2 = (const WebTexture*)TerrainTileRenderable::s_channelTexture2;
				const WebTexture* tex3 = (const WebTexture*)TerrainTileRenderable::s_channelTexture3;
				const WebTexture* tex4 = (const WebTexture*)TerrainTileRenderable::s_channelTexture4;

				// blendmap
				_shader.setUniform1i(_shader.getUniformLocation("tex_blendmap"), 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex_blendmap->getID());

				// channel 0
				_shader.setUniform1i(_shader.getUniformLocation("tex_channel_black"), 1);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, tex0->getID());
				// channel 1
				_shader.setUniform1i(_shader.getUniformLocation("tex_channel_red"), 2);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, tex1->getID());
				// channel 2
				_shader.setUniform1i(_shader.getUniformLocation("tex_channel_green"), 3);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, tex2->getID());
				// channel 3
				_shader.setUniform1i(_shader.getUniformLocation("tex_channel_blue"), 4);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, tex3->getID());
				// channel 4
				_shader.setUniform1i(_shader.getUniformLocation("tex_channel_alpha"), 5);
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, tex4->getID());

				glDrawElements(GL_TRIANGLES, instanceIndexCount * batch.getInstanceCount(), GL_UNSIGNED_SHORT, 0);
				
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


				// JUST TEMPORARELY FREE HERE!!!
				batch.clear();
			}
		}



		void WebTerrainRenderer::allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength)
		{
			std::vector<float> vertexData(maxBatchLength);
			std::vector<PK_ushort> indices(maxBatchLength);


			const int vertexCount = 4;
			int vertexIndex = 0;
			for (int i = 0; i < maxBatchLength; i += 6)
			{
				if (i + 5 >= indices.size())
					break;

				indices[i] =	 0 + vertexIndex;
				indices[i + 1] = 1 + vertexIndex;
				indices[i + 2] = 2 + vertexIndex;
				indices[i + 3] = 2 + vertexIndex;
				indices[i + 4] = 3 + vertexIndex;
				indices[i + 5] = 0 + vertexIndex;

				vertexIndex += vertexCount;
			}

			for (int i = 0; i < maxBatchCount; ++i)
			{
				WebVertexBuffer* vb = new WebVertexBuffer(vertexData, VertexBufferUsage::PK_BUFFER_USAGE_DYNAMIC);
				WebIndexBuffer* ib = new WebIndexBuffer(indices);

				_batches.push_back({ entryLength, maxBatchLength, {vb}, ib });
			}
		}
	}
}