
#include "WebGUIRenderer.h"
#include "shaders/sources/WebTestShader.h"

#include "WebFontRenderer.h"

#include "../../../core/Timing.h"

#include "../../../Common.h"

namespace pk
{
	namespace web
	{

		static std::string s_vertexSource = R"(
                precision mediump float;
                
                attribute vec2 position;
                attribute vec2 uv;
                attribute vec4 color;
                
				uniform mat4 projectionMatrix;
			
				varying vec2 var_uv;
				varying vec4 var_color;

                void main()
                {
					gl_Position = projectionMatrix * vec4(position, 0, 1.0);
					var_uv = uv;
					var_color = color;
				}
            )";

		static std::string s_fragmentSource = R"(
                precision mediump float;
                
				varying vec2 var_uv;
				varying vec4 var_color;
				
				uniform sampler2D textureSampler;
				
                void main()
                {
					vec4 texColor = texture2D(textureSampler, var_uv);
                    //float fr = mix(texColor.r, var_color.r, var_color.r)
					gl_FragColor = texColor * var_color;
                }
            )";
		
		WebGUIRenderer::WebGUIRenderer() : 
			_shader(s_vertexSource, s_fragmentSource)
		{
			_vertexAttribLocation_pos = _shader.getAttribLocation("position");
			_vertexAttribLocation_uv = _shader.getAttribLocation("uv");
			_vertexAttribLocation_color = _shader.getAttribLocation("color");

			_uniformLocation_projMat = _shader.getUniformLocation("projectionMatrix");
			_uniformLocation_texSampler = _shader.getUniformLocation("textureSampler");

			// Create default texture, if we want to just draw a colored rect
			char* defaultTextureData = new char[1 * 4];
			memset(defaultTextureData, 255, 4);
			_defaultTexture = new WebTexture(
				defaultTextureData, 1, 1, 4,
				{
					TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_NEAR,
					TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_REPEAT
				}
			);
			delete[] defaultTextureData;

			const int maxBatchInstanceCount = 256;
			const int batchInstanceDataLen = 8 * 4;
			allocateBatches(4, maxBatchInstanceCount * batchInstanceDataLen, batchInstanceDataLen);
		}
		
		WebGUIRenderer::~WebGUIRenderer()
		{
			for (BatchData& batch : _batches)
			{
				batch.destroy();
			}

			delete _defaultTexture;
		}

		// submit renderable component for rendering (batch preparing, before rendering)
		void WebGUIRenderer::submit(const Component* const renderableComponent, const mat4& transformation)
		{
			// <#M_DANGER>
			const GUIRenderable* const  renderable = (const GUIRenderable* const)(renderableComponent);
			const vec2 pos(transformation[0 + 3 * 4], transformation[1 + 3 * 4]);
			const vec2 scale(transformation[0 + 0 * 4], transformation[1 + 1 * 4]);
			const vec3& color = renderable->color;

			const vec2 uv_v0(0, 1);
			const vec2 uv_v1(0, 0);
			const vec2 uv_v2(1, 0);
			const vec2 uv_v3(1, 1);

			std::vector<float> dataToSubmit{ 
				pos.x,			 pos.y,			 	uv_v0.x, uv_v0.y,	color.x,color.y,color.z,1.0f,
				pos.x,			 pos.y - scale.y,	uv_v1.x, uv_v1.y,	color.x,color.y,color.z,1.0f,
				pos.x + scale.x, pos.y - scale.y,	uv_v2.x, uv_v2.y,	color.x,color.y,color.z,1.0f,
				pos.x + scale.x, pos.y,				uv_v3.x, uv_v3.y,	color.x,color.y,color.z,1.0f
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
					batch.occupy(renderable->textureID);
					batch.insertInstanceData(0, dataToSubmit);
					batch.addNewInstance();
					//Debug::log("created new batch");
					return;
				}
				else if (batch.ID == renderable->textureID)
				{
					batch.insertInstanceData(0, dataToSubmit);
					batch.addNewInstance();
					//Debug::log("assigned to existing");
					return;
				}
			}
		}


		void WebGUIRenderer::render(const Camera& cam)
		{
			mat4 projectionMatrix = cam.getProjMat2D();

			glUseProgram(_shader.getProgramID());

			// all common uniforms..
			_shader.setUniform(_uniformLocation_projMat, projectionMatrix);

			//glDisable(GL_DEPTH_TEST);
			glDepthFunc(GL_ALWAYS);

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

				int stride = sizeof(float) * 8;
				glBindBuffer(GL_ARRAY_BUFFER, vb->getID());

				// position vertex attrib
				glEnableVertexAttribArray(_vertexAttribLocation_pos);
				glVertexAttribPointer(_vertexAttribLocation_pos, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, stride, 0);

				// uv coord vertex attrib
				glEnableVertexAttribArray(_vertexAttribLocation_uv);
				glVertexAttribPointer(_vertexAttribLocation_uv, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 2));
				
				// color vertex attrib
				glEnableVertexAttribArray(_vertexAttribLocation_color);
				glVertexAttribPointer(_vertexAttribLocation_color, 4, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 4));

				// Bind texture only if we have one... (greater than 0 indicates that)
				int textureID = batch.ID;
				if(textureID > 0)
				{
				}
				else
				{
					// ..otherwise user our "default texture"
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, _defaultTexture->getID());
				}

				glDrawElements(GL_TRIANGLES, instanceIndexCount * batch.getInstanceCount(), GL_UNSIGNED_SHORT, 0);

				glBindTexture(GL_TEXTURE_2D, 0);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


				// JUST TEMPORARELY FREE HERE!!!
				batch.clear();
			}
		}



		void WebGUIRenderer::allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength)
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
