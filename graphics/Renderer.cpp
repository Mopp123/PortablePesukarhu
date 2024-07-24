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
        freeDescriptorSets();
        for (Batch* pBatch : _batches)
            delete pBatch;
    }

    bool BatchContainer::addData(const void* data, size_t dataSize, PK_id batchIdentifier)
    {
        for (Batch* pBatch : _batches)
        {
            if (pBatch->isFull())
                continue;

            if (pBatch->isOccupied() && pBatch->getIdentifier() == batchIdentifier)
            {
                pBatch->addData(data, dataSize);
                return true;
            }
            else if (!pBatch->isOccupied())
            {
                pBatch->occupy(batchIdentifier);
                pBatch->addData(data, dataSize);
                if (_batchDescriptorSets.find(batchIdentifier) == _batchDescriptorSets.end())
                    _batchDescriptorSets[batchIdentifier] = { {}, {} };
                return true;
            }
        }
        Debug::log(
            "@BatchContainer::addData "
            "All batches were full!",
            Debug::MessageType::PK_ERROR
        );
        return false;
    }

    void BatchContainer::createDescriptorSets(
        PK_id batchIdentifier,
        const DescriptorSetLayout * const pTextureDescriptorSetLayout,
        Texture_new* pTexture,
        const DescriptorSetLayout * const pUboDescriptorSetLayout,
        const std::vector<Buffer*>& ubo
    )
    {
        std::unordered_map<PK_id, BatchDescriptorSets>::iterator it = _batchDescriptorSets.find(batchIdentifier);

        if (it != _batchDescriptorSets.end())
        {
            BatchDescriptorSets& sets = it->second;

            const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
            uint32_t swapchainImages = pSwapchain->getImageCount();
            std::vector<DescriptorSet*>& textureDescriptorSet = sets._textureDescriptorSet;
            std::vector<DescriptorSet*>& uboDescriptorSet = sets._uboDescriptorSet;

            if (textureDescriptorSet.size() > 0)
                Debug::log("___TEST___ERROR ATTEMPTED TO CREATE DUPLICATE");

            if (pTextureDescriptorSetLayout && pTexture)
                textureDescriptorSet.resize(swapchainImages);
            if (pUboDescriptorSetLayout && !ubo.empty())
            {
                if (ubo.size() == swapchainImages)
                {
                    uboDescriptorSet.resize(swapchainImages);
                }
                else
                {
                    Debug::log(
                        "@BatchContainer::createDescriptorSets "
                        "Mismatch between swapchain image count and provided uniform buffers! "
                        "Swapchain images: " + std::to_string(swapchainImages) + " "
                        "provided uniform buffers: " + std::to_string(ubo.size()) + " "
                        "Required to provide uniform buffer for each swapchain image!",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                }
            }
            for (uint32_t i = 0; i < swapchainImages; ++i)
            {
                if (!textureDescriptorSet.empty())
                {
                    DescriptorSet* pDescriptorSet = new DescriptorSet(
                        *pTextureDescriptorSetLayout,
                        1,
                        { pTexture }
                    );
                    textureDescriptorSet[i] = pDescriptorSet;
                }
                if (!uboDescriptorSet.empty())
                {
                    DescriptorSet* pDescriptorSet = new DescriptorSet(
                        *pUboDescriptorSetLayout,
                        1,
                        { ubo[i] }
                    );
                    uboDescriptorSet[i] = pDescriptorSet;
                }
            }
            Debug::log(
                "@Batch::createDescriptorSets "
                "Created " + std::to_string(textureDescriptorSet.size()) + " texture "
                "and " + std::to_string(uboDescriptorSet.size()) + " ubo descriptor sets successfully"
            );
        }
        else
        {
            Debug::log(
                "@BatchContainer::createDescriptorSets "
                "Failed to find batch for identifier: " + std::to_string(batchIdentifier),
                Debug::MessageType::PK_FATAL_ERROR
            );
        }
    }

    void BatchContainer::freeDescriptorSets()
    {
        std::unordered_map<PK_id, BatchDescriptorSets>::iterator it;
        for (it = _batchDescriptorSets.begin(); it != _batchDescriptorSets.end(); ++it)
        {
            for (DescriptorSet* d : it->second._textureDescriptorSet)
                delete d;
            for (DescriptorSet* d : it->second._uboDescriptorSet)
                delete d;
            it->second._textureDescriptorSet.clear();
            it->second._uboDescriptorSet.clear();
        }
        Debug::log("@BatchContainer::freeDescriptorSets Descriptor sets freed");
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

    bool BatchContainer::hasDescriptorSets(PK_id batchIdentifier) const
    {
        std::unordered_map<PK_id, BatchDescriptorSets>::const_iterator it = _batchDescriptorSets.find(batchIdentifier);
        if (it != _batchDescriptorSets.end())
            return !it->second._textureDescriptorSet.empty() || !it->second._uboDescriptorSet.empty();
        return false;
    }

    const DescriptorSet* BatchContainer::getTextureDescriptorSet(PK_id batchIdentifier, uint32_t index) const
    {
        std::unordered_map<PK_id, BatchDescriptorSets>::const_iterator it = _batchDescriptorSets.find(batchIdentifier);
        if (it != _batchDescriptorSets.end())
        {
            const std::vector<DescriptorSet*>& sets = it->second._textureDescriptorSet;
            if (index < sets.size())
            {
                return sets[index];
            }
            else
            {
                Debug::log(
                    "@BatchContainer::getTextureDescriptorSet "
                    "Index: " + std::to_string(index) + " out of bounds! "
                    "descriptor sets length: " + std::to_string(sets.size()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
        }
        Debug::log(
            "@BatchContainer::getTextureDescriptorSet "
            "No descriptor sets found for identifier: " + std::to_string(batchIdentifier),
            Debug::MessageType::PK_FATAL_ERROR
        );
        return nullptr;
    }

    const DescriptorSet* BatchContainer::getUboDescriptorSet(PK_id batchIdentifier, uint32_t index) const
    {
        std::unordered_map<PK_id, BatchDescriptorSets>::const_iterator it = _batchDescriptorSets.find(batchIdentifier);
        if (it != _batchDescriptorSets.end())
        {
            const std::vector<DescriptorSet*>& sets = it->second._uboDescriptorSet;
            if (index < sets.size())
            {
                return sets[index];
            }
            else
            {
                Debug::log(
                    "@BatchContainer::getUboDescriptorSet "
                    "Index: " + std::to_string(index) + " out of bounds! "
                    "descriptor sets length: " + std::to_string(sets.size()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
        }
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
        freeDescriptorSets();
        initPipeline();
    }
}
