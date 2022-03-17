
#include "WebGUIRenderer.h"
#include "shaders/sources/WebTestShader.h"

#include "../../../core/Timing.h"

#include "../../../Common.h"

namespace pk
{
	namespace web
	{

		static std::string s_vertexSource = R"(
                precision mediump float;
                
                attribute vec2 position;
                
                void main()
                {
                   gl_Position = vec4(position, 0, 1.0);
                }
            )";

		static std::string s_fragmentSource = R"(
                precision mediump float;
                
                void main()
                {
                    gl_FragColor = vec4(0,1,0,1);
                }
            )";

		
		
		WebGUIRenderer::WebGUIRenderer() : 
			_shader(s_vertexSource, s_fragmentSource)
		{
			_vertexAttribLocation_pos = _shader.getAttribLocation("position");
			

			std::vector<PK_float> vertexPositions = {
				0, 0,
				0, -0.5f,
				0.5f, -0.5f,
				0.5f, 0
			};

			std::vector<PK_float> translation = {
				-1,0,
				-1,0,
				-1,0,
				-1,0
			};

			std::vector<PK_ushort> indices = {
				0,1,2,
				2,3,0
			};


			allocateBatches(4, PK_LIMITS_DRAWCALL_MAX_VERTEX_COUNT / 8, 8);
		}
		
		WebGUIRenderer::~WebGUIRenderer()
		{
		}

		// submit renderable component for rendering (batch preparing, before rendering)
		void WebGUIRenderer::submit(const Component* const renderableComponent)
		{
			
			// <#M_DANGER>
			const GUIRenderable* const  renderable = (const GUIRenderable* const)(renderableComponent);


			/*
				0, 0,
				0, -0.5f,
				0.5f, -0.5f,
				0.5f, 0
			*/
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


		void WebGUIRenderer::render()
		{
			glUseProgram(_shader.getProgramID());


			for (GUIBatchData& batch : _batches)
			{
				if (batch.isFree)
					continue;

				
				WebVertexBuffer* vb = batch.vertexBuffer;
				WebIndexBuffer* ib = batch.indexBuffer;
				// hardcoded just as temp..
				const GLsizei instanceIndexCount = 6;

				glEnableVertexAttribArray(_vertexAttribLocation_pos);
				glBindBuffer(GL_ARRAY_BUFFER, vb->getID());

				// Update the vertex buffer's data
				const std::vector<float>& updatedVBData = vb->getData();
				vb->update(updatedVBData, 0, sizeof(float) * batch.instanceDataLen * batch.instanceCount);
				glVertexAttribPointer(_vertexAttribLocation_pos, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, sizeof(PK_float) * 2, 0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getID());
				glDrawElements(GL_TRIANGLES, instanceIndexCount * batch.instanceCount, GL_UNSIGNED_SHORT, 0);

				Debug::log("TEST:" + std::to_string(batch.instanceCount) + ")");

				//Debug::log("Rendering a batch(instance count: " + std::to_string(batch.instanceCount) + ")");

				// JUST TEMPORARELY FREE HERE!!!
				batch.clear();
			}
		}



		void WebGUIRenderer::allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength)
		{
			Debug::log("Allocating batches. maxBatchCount: " + std::to_string(maxBatchCount) + ", maxBatchLength: " + std::to_string(maxBatchLength) + ", entryLength" + std::to_string(entryLength));

			std::vector<float> vertexData(maxBatchLength * entryLength);

			std::vector<PK_ushort> indices(maxBatchLength * 6);


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

			/*
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;

			indices[6] =	4;
			indices[7] =	5;
			indices[8] =	6;
			indices[9] =	6;
			indices[10] =	7;
			indices[11] =	4;
			*/

			for (int i = 0; i < maxBatchCount; ++i)
			{
				WebVertexBuffer* vb = new WebVertexBuffer(vertexData, VertexBufferUsage::PK_BUFFER_USAGE_DYNAMIC);
				WebIndexBuffer* ib = new WebIndexBuffer(indices);

				_batches.push_back({ entryLength, maxBatchLength, vb, ib });
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

			batch.vertexBuffer->accessRawData()[batch.currentDataPtr] = data[0];
			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 1] = data[1];

			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 2] = data[2];
			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 3] = data[3];

			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 4] = data[4];
			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 5] = data[5];

			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 6] = data[6];
			batch.vertexBuffer->accessRawData()[batch.currentDataPtr + 7] = data[7];


			batch.currentDataPtr += batch.instanceDataLen;
			batch.instanceCount++;
		}
	}
}