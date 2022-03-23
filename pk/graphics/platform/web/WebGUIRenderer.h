#pragma once

#include "../../Renderer.h"
#include "../../../ecs/components/renderable/GUIRenderable.h"
#include "WebBuffers.h"
#include "shaders/WebShader.h"

#include "../../../core/Debug.h"

#include <vector>
#include <unordered_map>

namespace pk
{
	namespace web
	{
		struct GUIBatchData
		{
			WebVertexBuffer* vertexBuffer = nullptr;
			WebVertexBuffer* vertexBuffer_uvs = nullptr;
			WebIndexBuffer* indexBuffer = nullptr;

			// length of a single instance's data (not to be confused with "instanceCount")
			int instanceDataLen = -1; // ..kind of like "data slot" for each vertex of each instance. For example: one vec2 for each of the quad's 4 vertices -> instanceDataLength=8
			int maxTotalBatchDataLen = 0;

			int ID = -1;
			bool isFree = true;
			int currentDataPtr = 0;
			
			int instanceCount = 0;

			// *Ownerships gets always transferred to this GUIBatchData
			GUIBatchData(int dataEntryLen, int totalDataLen, WebVertexBuffer* vb, WebVertexBuffer* vb_uvs, WebIndexBuffer* ib) :
				instanceDataLen(dataEntryLen), maxTotalBatchDataLen(totalDataLen), vertexBuffer(vb), vertexBuffer_uvs(vb_uvs), indexBuffer(ib)
			{}
			// *Ownerships gets always transferred to this GUIBatchData
			GUIBatchData(const GUIBatchData& other) :
				instanceDataLen(other.instanceDataLen),
				maxTotalBatchDataLen(other.maxTotalBatchDataLen), 
				vertexBuffer(other.vertexBuffer), 
				vertexBuffer_uvs(other.vertexBuffer_uvs),
				indexBuffer(other.indexBuffer)
			{

			}

			void clear()
			{
				ID = -1;
				isFree = true;
				currentDataPtr = 0;
				instanceCount = 0;
			}

			inline bool isFull() const { return currentDataPtr >= maxTotalBatchDataLen; }
		};

		class WebGUIRenderer : public Renderer
		{
		private:

			WebShader _shader;

			PK_int _vertexAttribLocation_pos = -1;
			PK_int _vertexAttribLocation_uv = -1;

			PK_int _uniformLocation_projMat = -1;
			PK_int _uniformLocation_texSampler = -1;
			
			std::vector<GUIBatchData> _batches;

		public:

			WebGUIRenderer();
			~WebGUIRenderer();

			// submit renderable component for rendering (batch preparing, before rendering)
			virtual void submit(const Component* const renderableComponent);

			virtual void render(mat4& projectionMatrix, mat4& viewMatrix);

			virtual void resize(int w, int h) {}

		private:

			void allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength);
			void occupyBatch(GUIBatchData& batch, int batchId);
			void addToBatch(GUIBatchData& batch, const std::vector<float>& data);
		};
	}

}