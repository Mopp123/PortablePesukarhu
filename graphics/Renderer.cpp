#include "Renderer.h"
#include "core/Application.h"
#include "core/Window.h"


namespace pk
{
    // TODO: Delete current BatchData shit..
    void BatchData::addNewInstance()
    {
        currentDataPtr += instanceDataLen;
        isFull = currentDataPtr >= maxTotalBatchDataLen;
    }
    void BatchData::insertInstanceData(int bufferIndex, const std::vector<float>& data)
    {
        VertexBuffer* vb = vertexBuffers[bufferIndex];
        /*float* dat = &vb->accessRawData()[0];
          memcpy(dat + currentDataPtr, &data[0], sizeof(float) * instanceDataLen);
          */
        vb->update(data, sizeof(float) * currentDataPtr, sizeof(float) * data.size());
    }


    Batch::Batch(size_t maxSize) :
        _maxSize(maxSize)
    {
        PK_byte* pInitialData = new PK_byte[_maxSize];
        memset(pInitialData, 0, _maxSize);
        _pBuffer = Buffer::create(
            pInitialData,
            1,
            maxSize,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT,
            true
        );
        delete[] pInitialData;
    }

    Batch::~Batch()
    {
        if (_pBuffer)
            delete _pBuffer;
    }

    void Batch::occupy(PK_id identifier)
    {
        _occupied = true;
        _identifier = identifier;
    }

    bool Batch::addData(const void* pData, size_t dataSize)
    {
        if (_occupiedSize + dataSize > _maxSize)
        {
            Debug::log(
                "@Batch::addData Batch was full!",
                Debug::MessageType::PK_WARNING
            );
            return false;
        }
        _pBuffer->update(pData, _writePos, dataSize);
        _writePos += dataSize;
        _occupiedSize += dataSize;
        _instanceCount++;
        return true;
    }

    void Batch::clear()
    {
        _writePos = 0;
        _occupiedSize = 0;
        _occupied = false;
        _identifier = 0;
        _instanceCount = 0;
        _pBuffer->clearHostSideBuffer();
    }


    BatchContainer::BatchContainer(
        size_t maxBatches,
        size_t batchSize
    )
    {
        _batches.resize(maxBatches);
        for (size_t i = 0; i < maxBatches; ++i)
            _batches[i] = new Batch(batchSize);
    }

    BatchContainer::~BatchContainer()
    {
        for (Batch* pBatch : _batches)
            delete pBatch;
    }

    void BatchContainer::addData(const void* data, size_t dataSize, PK_id batchIdentifier)
    {
        for (Batch* pBatch : _batches)
        {
            if (pBatch->isFull())
                continue;

            if (pBatch->isOccupied() && pBatch->getIdentifier() == batchIdentifier)
            {
                pBatch->addData(data, dataSize);
                return;
            }
            else if (!pBatch->isOccupied())
            {
                pBatch->occupy(batchIdentifier);
                pBatch->addData(data, dataSize);
                return;
            }
        }
        Debug::log(
            "@BatchContainer::addData "
            "All batches were full!",
            Debug::MessageType::PK_ERROR
        );

        /*
        Batch* pFoundBatch = nullptr;
        std::unordered_map<PK_id, Batch*>::iterator it = _occupiedBatches.find(batchIdentifier);
        if (it != _occupiedBatches.end())
        {
            pFoundBatch = it->second;
        }
        else
        {
            for (Batch* pBatch : _batches)
            {
                if (!pBatch->isOccupied() && !pBatch->isFull())
                {
                    pFoundBatch = pBatch;
                    _occupiedBatches[batchIdentifier] = pFoundBatch;
                    break;
                }
            }
        }

        if (pFoundBatch)
        {
            pFoundBatch->addData(data, dataSize);
        }
        else
        {
            Debug::log(
                "@BatchContainer::addData "
                "All batches were full!",
                Debug::MessageType::PK_ERROR
            );
        }
        */
    }

    void BatchContainer::clear()
    {
        for (Batch* pBatch : _batches)
            pBatch->clear();
        _occupiedBatches.clear();
    }

    const Batch* BatchContainer::getBatch(PK_id batchIdentifier) const
    {
        std::unordered_map<PK_id, Batch*>::const_iterator it = _occupiedBatches.find(batchIdentifier);
        if (it != _occupiedBatches.end())
            return it->second;
        Debug::log(
            "@BatchContainer::getBatch "
            "Couldn't find batch with identifier: " + std::to_string(batchIdentifier),
            Debug::MessageType::PK_ERROR
        );
        return nullptr;
    }


    Renderer::Renderer()
    {
        _pPipeline = Pipeline::create();

        const Application* pApp = Application::get();
        if (!pApp)
        {
            Debug::log(
                "@Renderer::Renderer Application was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        const Window* pWindow = pApp->getWindow();
        if (!pWindow)
        {
            Debug::log(
                "@Renderer::Renderer Application Window was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        const Swapchain* pSwapchain = pWindow->getSwapchain();
        if (!pSwapchain)
        {
            Debug::log(
                "@Renderer::Renderer Swapchain was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        // Create command buffers
        // These will persist through Renderer's lifetimr
        // NOTE: These will be "freed" and "re allocated" if
        // resizing window but the actual CommandBuffer objects
        // will remain. Only their internal content will be changed!
        uint32_t swapchainImages = pSwapchain->getImageCount();
        _pCommandBuffers[RenderPassType::RENDER_PASS_DIFFUSE] = CommandBuffer::create(swapchainImages);
    }

    Renderer::~Renderer()
    {
        delete _pPipeline;
        std::unordered_map<RenderPassType, std::vector<CommandBuffer*>>::const_iterator bIt;
        for (bIt = _pCommandBuffers.begin(); bIt != _pCommandBuffers.end(); ++bIt)
        {
            for (CommandBuffer* pBuf : (*bIt).second)
                delete pBuf;
        }
    }

    void Renderer::handleWindowResize()
    {
        _pPipeline->cleanUp();
        std::unordered_map<RenderPassType, std::vector<CommandBuffer*>>::const_iterator bIt;
        for (bIt = _pCommandBuffers.begin(); bIt != _pCommandBuffers.end(); ++bIt)
        {
            for (CommandBuffer* pBuf : (*bIt).second)
            {
                pBuf->free();
                pBuf->allocate();
            }
        }
        recreateDescriptorSets();
        initPipeline();
    }
}
