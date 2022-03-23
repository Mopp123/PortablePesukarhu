
#include "WebGUIRenderer.h"
#include "shaders/sources/WebTestShader.h"
#include "WebTexture.h"

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
                
				uniform mat4 projectionMatrix;
			
				varying vec2 var_uv;

                void main()
                {
					gl_Position = projectionMatrix * vec4(position, 0, 1.0);
					var_uv = uv;
				}
            )";

		static std::string s_fragmentSource = R"(
                precision mediump float;
                
				varying vec2 var_uv;
				
				uniform sampler2D textureSampler;
				
                void main()
                {
                    gl_FragColor = texture2D(textureSampler, var_uv);
                }
            )";

		
		static WebTexture* s_testTexture = nullptr;
		
		WebGUIRenderer::WebGUIRenderer() : 
			_shader(s_vertexSource, s_fragmentSource)
		{
			_vertexAttribLocation_pos = _shader.getAttribLocation("position");
			_vertexAttribLocation_uv = _shader.getAttribLocation("uv");

			_uniformLocation_projMat = _shader.getUniformLocation("projectionMatrix");
			_uniformLocation_texSampler = _shader.getUniformLocation("textureSampler");

			s_testTexture = new WebTexture("assets/test.png");

			allocateBatches(4, PK_LIMITS_DRAWCALL_MAX_VERTEX_DATA_LEN / 8, 8);
		}
		
		WebGUIRenderer::~WebGUIRenderer()
		{
			for (GUIBatchData& batch : _batches)
			{
				delete batch.vertexBuffer;
				delete batch.vertexBuffer_uvs;
				delete batch.indexBuffer;
			}

			delete s_testTexture;
		}

		// submit renderable component for rendering (batch preparing, before rendering)
		void WebGUIRenderer::submit(const Component* const renderableComponent)
		{
			// <#M_DANGER>
			const GUIRenderable* const  renderable = (const GUIRenderable* const)(renderableComponent);

			std::vector<float> dataToSubmit{ 
				renderable->pos.x, renderable->pos.y, 
				renderable->pos.x, renderable->pos.y - renderable->scale.y,
				renderable->pos.x + renderable->scale.x, renderable->pos.y - renderable->scale.y,
				renderable->pos.x + renderable->scale.x, renderable->pos.y
			};

			// Find suitable batch
			for (int i = 0; i < _batches.size(); ++i)
			{
				
				GUIBatchData& batch = _batches[i];
				if (batch.isFull())
				{
					//Debug::log("Batch was full!");
					continue;
				}

				if (batch.isFree)
				{
					occupyBatch(batch, renderable->textureID);
					addToBatch(batch, dataToSubmit);
					//Debug::log("created new batch");
					return;
				}
				else if (batch.ID == renderable->textureID)
				{
					addToBatch(batch, dataToSubmit);
					//Debug::log("assigned to existing");
					return;
				}
			}
		}


		void WebGUIRenderer::render(mat4& projectionMatrix, mat4& viewMatrix)
		{
			/*glUseProgram(_shader.getProgramID());

			// all common uniforms..
			_shader.setUniform(_uniformLocation_projMat, projectionMatrix);


			glDisable(GL_CULL_FACE);
			//glCullFace(GL_BACK);

			for (GUIBatchData& batch : _batches)
			{
				if (batch.isFree)
					continue;

				
				WebVertexBuffer* vb = batch.vertexBuffer;
				WebVertexBuffer* vb_uv = batch.vertexBuffer_uvs;

				WebIndexBuffer* ib = batch.indexBuffer;
				// hardcoded just as temp..
				const GLsizei instanceIndexCount = 6;

				glEnableVertexAttribArray(_vertexAttribLocation_pos);
				glBindBuffer(GL_ARRAY_BUFFER, vb->getID());

				// Update the vertex buffer's data
				const std::vector<float>& updatedVBData = vb->getData();
				vb->update(updatedVBData, 0, sizeof(float) * batch.instanceDataLen * batch.instanceCount);
				glVertexAttribPointer(_vertexAttribLocation_pos, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, sizeof(PK_float) * 2, 0);

				// uv coord vertex buffer
				glEnableVertexAttribArray(_vertexAttribLocation_uv);
				glBindBuffer(GL_ARRAY_BUFFER, vb_uv->getID());
				glVertexAttribPointer(_vertexAttribLocation_uv, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, sizeof(PK_float) * 2, 0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getID());
				
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, s_testTexture->getID());
				
				glUniform1i(_uniformLocation_texSampler, 0);

				glDrawElements(GL_TRIANGLES, instanceIndexCount * batch.instanceCount, GL_UNSIGNED_SHORT, 0);

				// JUST TEMPORARELY FREE HERE!!!
				batch.clear();
			}*/
		}



		void WebGUIRenderer::allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength)
		{
			Debug::log("Allocating batches. maxBatchCount: " + std::to_string(maxBatchCount) + ", maxBatchLength: " + std::to_string(maxBatchLength) + ", entryLength" + std::to_string(entryLength));

			std::vector<float> vertexData(maxBatchLength * entryLength);

			std::vector<float> vertexData_uvs(maxBatchLength * 2);

			std::vector<PK_ushort> indices(maxBatchLength * 6);

			for (int i = 0; i < vertexData_uvs.size(); i += 8)
			{
				vertexData_uvs[i] = 0;
				vertexData_uvs[i + 1] = 1;

				vertexData_uvs[i + 2] = 0;
				vertexData_uvs[i + 3] = 0;

				vertexData_uvs[i + 4] = 1;
				vertexData_uvs[i + 5] = 0;

				vertexData_uvs[i + 6] = 1;
				vertexData_uvs[i + 7] = 1;
			}

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
				WebVertexBuffer* vb_uvs = new WebVertexBuffer(vertexData_uvs, VertexBufferUsage::PK_BUFFER_USAGE_STATIC);
				WebIndexBuffer* ib = new WebIndexBuffer(indices);

				_batches.push_back({ entryLength, maxBatchLength, vb, vb_uvs, ib });
			}
		}
		

		void WebGUIRenderer::occupyBatch(GUIBatchData& batch, int batchId)
		{
			batch.ID = batchId;
			batch.isFree = false;
		}

		void WebGUIRenderer::addToBatch(GUIBatchData& batch, const std::vector<float>& data)
		{
			//memcpy((&batch.vertexBuffer->accessRawData()[0]) + batch.currentDataPtr, &data[0], sizeof(float) * batch.instanceDataLen);
			/*
			batch.vertexBuffer->accessRawData()[batch.currentDataPtr] = data[0];
			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 1] = data[1];

			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 2] = data[2];
			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 3] = data[3];

			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 4] = data[4];
			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 5] = data[5];

			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 6] = data[6];
			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 7] = data[7];


			batch.currentDataPtr += batch.instanceDataLen;
			batch.instanceCount++;*/
		}
	}
}