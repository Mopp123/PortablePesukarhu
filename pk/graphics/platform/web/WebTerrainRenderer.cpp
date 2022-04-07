
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

                void main()
                {	
					gl_Position = projectionMatrix * viewMatrix * mat4(1.0) * vec4(position, 1.0);
					var_uv = uv;

					if(position.y > 0.0)
					{
						var_normal = vec3(uv.x, 1.0, uv.y) * position.y;
					}					
					else
					{
						var_normal = vec3(0,1.0,0);
					}
				}
            )";

		static std::string s_fragmentSource = R"(
                precision mediump float;
                
				varying vec2 var_uv;
				varying vec3 var_normal;
				
				uniform sampler2D textureSampler;
				uniform vec3 dirLight_dir;
				
                void main()
                {

					vec3 lightDir = normalize(dirLight_dir);
					float diffFactor = clamp(dot(-lightDir, normalize(var_normal)), 0.0, 1.0);


					vec4 texColor = texture2D(textureSampler, var_uv);

					vec4 finalColor = texColor * diffFactor;


					gl_FragColor = finalColor;
	              }
            )";
		
		WebTerrainRenderer::WebTerrainRenderer() : 
			_shader(s_vertexSource, s_fragmentSource)
		{
			_vertexAttribLocation_pos = _shader.getAttribLocation("position");
			_vertexAttribLocation_uv = _shader.getAttribLocation("uv");
			
			_uniformLocation_projMat = _shader.getUniformLocation("projectionMatrix");
			_uniformLocation_viewMat = _shader.getUniformLocation("viewMatrix");
			_uniformLocation_texSampler = _shader.getUniformLocation("textureSampler");

			_uniformLocation_dirLight_dir = _shader.getUniformLocation("dirLight_dir");
			//_uniformLocation_dirLight_color = _shader.getUniformLocation("dirLight_color");

			
			// Load tex atlas
			_textureAtlas = new WebTexture(
				"assets/Terrain.png",
				{
					TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_NEAR,
					TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
					2
				}
			);

			const int maxBatchInstanceCount = 256;
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



		static float S_TEST_changeCooldown = 0.0f;
		// submit renderable component for rendering (batch preparing, before rendering)
		void WebTerrainRenderer::submit(const Component* const renderableComponent, const mat4& transformation)
		{
			// <#M_DANGER>
			const TerrainTileRenderable* const  renderable = (const TerrainTileRenderable* const)(renderableComponent);
			
			const float xPos = (float)renderable->tileMapX;
			const float zPos = (float)renderable->tileMapY;
			
			const float scale = renderable->scale;

			float height_tl = renderable->vertexHeights[0];
			float height_tr = renderable->vertexHeights[1];
			float height_bl = renderable->vertexHeights[2];
			float height_br = renderable->vertexHeights[3];

			float combinedHeight = height_tl + height_tr + height_bl + height_br;

			// We need to negate a single pixel on some uvs, otherwise ugly artifacts on tiles' borders
			const float pixelUvSpace = (1.0f / _texAtlasWidth) /2.0f;
			const float texAtlasRows = 8.0f;
			
			vec2 texOffset(0, 0);
			if (combinedHeight <= 0.0f)
				texOffset = vec2(0.0f, 1.0f);

			vec2 uv_v0(0, 1);
			vec2 uv_v1(0, 0);
			vec2 uv_v2(1, 0);
			vec2 uv_v3(1, 1);

			uv_v0 = (uv_v0 + texOffset) / texAtlasRows;
			uv_v1 = (uv_v1 + texOffset) / texAtlasRows;
			uv_v2 = (uv_v2 + texOffset) / texAtlasRows;
			uv_v3 = (uv_v3 + texOffset) / texAtlasRows;

			const int batchIdentifier = 1; // atm everyone will go into the same batch, FOR TESTING PURPOSES!
			
			std::vector<float> dataToSubmit{ 
				xPos,				height_tl,		zPos,				uv_v0.x, uv_v0.y - pixelUvSpace,			
				xPos,				height_bl,		zPos + scale,		uv_v1.x, uv_v1.y,							
				xPos + scale,		height_br,		zPos + scale,		uv_v2.x - pixelUvSpace, uv_v2.y,			
				xPos + scale,		height_tr,		zPos,				uv_v3.x - pixelUvSpace, uv_v3.y - pixelUvSpace
			};
			
			

			// Find suitable batch
			for (int i = 0; i < _batches.size(); ++i)
			{
				BatchData& batch = _batches[i];
				if (batch.isFull())
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
			Transform* camTransform = (Transform*)scene->getComponent(cam.getEntity(), ComponentType::PK_TRANSFORM);
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
				
				

				// Bind texture
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, _textureAtlas->getID());
				

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