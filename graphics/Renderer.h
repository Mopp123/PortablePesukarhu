#pragma once

#include "../ecs/components/Component.h"
#include "../ecs/components/Camera.h"
#include "shaders/Shader.h"
#include "../utils/pkmath.h"

#include "Buffers.h"
#include <vector>

namespace pk
{
	/*
	*	NOTE!
	*		* Currently all vertex buffers inside BatchData HAS TO BE THE SAME SIZE (currentDataPtr advancing problem...)
	*/
	struct BatchData
	{
		std::vector<VertexBuffer*> vertexBuffers;
		IndexBuffer* indexBuffer = nullptr;

		// length of a single instance's data (not to be confused with "instanceCount")
		int instanceDataLen = -1; // ..kind of like "data slot" for each vertex of each instance. For example: one vec2 for each of the quad's 4 vertices -> instanceDataLength=8
		int maxTotalBatchDataLen = 0; // total number of "float" spots to distribute for each instance

		int ID = -1; // Identifier, how we find suitable batch for a renderable (can be anythin u want)
		bool isFree = true; // If theres nobody yet in this batch (completely empty -> available for occupying for new batch)(DON'T confuse with fullness:D)
		bool isFull = false;
		int currentDataPtr = 0; // When submitting to this batch, we use this to determine, at which position of the batch, we want to insert new data

		// *Ownerships gets always transferred to this BatchData instance
		BatchData(int dataEntryLen, int totalDataLen, const std::vector<VertexBuffer*>& vbs, IndexBuffer* ib) :
			instanceDataLen(dataEntryLen), maxTotalBatchDataLen(totalDataLen), vertexBuffers(vbs), indexBuffer(ib)
		{}
		// *Ownerships gets always transferred to this BatchData
		BatchData(const BatchData& other) :
			instanceDataLen(other.instanceDataLen),
			maxTotalBatchDataLen(other.maxTotalBatchDataLen),
			vertexBuffers(other.vertexBuffers),
			indexBuffer(other.indexBuffer)
		{}

		void destroy()
		{
			for (VertexBuffer* vb : vertexBuffers)
				delete vb;

			delete indexBuffer;
		}

		void clear()
		{
			ID = -1;
			isFree = true;
			isFull = false;
			currentDataPtr = 0;
		}

		inline int getInstanceCount() const { return currentDataPtr / instanceDataLen; }

		void occupy(int batchId)
		{
			ID = batchId;
			isFree = false;
		}

		void addNewInstance()
		{
			currentDataPtr += instanceDataLen;
			isFull = currentDataPtr >= maxTotalBatchDataLen;
		}
		void insertInstanceData(int bufferIndex, const std::vector<float>& data)
		{
			VertexBuffer* vb = vertexBuffers[bufferIndex];
			/*float* dat = &vb->accessRawData()[0];
			memcpy(dat + currentDataPtr, &data[0], sizeof(float) * instanceDataLen);
			*/
			vb->update(data, sizeof(float) * currentDataPtr, sizeof(float) * data.size());
		}
	};

	class Renderer
	{
	public:

		Renderer() {}
		virtual ~Renderer() {}

		// submit renderable component for rendering (batch preparing, before rendering)
		virtual void submit(const Component* const renderableComponent, const mat4& transformation) = 0;

		virtual void render(const Camera& cam) = 0; //*why the fuk proj and view matrices not const?

		virtual void resize(int w, int h) = 0;

		virtual void beginFrame() {}
		virtual void beginRenderPass(){}
		virtual void endRenderPass() {}
		virtual void endFrame() {}

	};

}