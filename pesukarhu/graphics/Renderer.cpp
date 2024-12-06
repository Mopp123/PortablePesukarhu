#include "Renderer.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/Window.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
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
            BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STREAM,
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
            {
                Debug::log(
                    "@BatchContainer::addData "
                    "Failed to add data using batchID: " + std::to_string(batchIdentifier) + " "
                    "Attempted batch instance count: " + std::to_string(pBatch->getInstanceCount())
                );
                continue;
            }

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
                    _batchDescriptorSets[batchIdentifier] = { {} };
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
        const DescriptorSetLayout * const pDescriptorSetLayout,
        const std::vector<const Texture*>& textures,
        const std::vector<const Buffer*>& ubo
    )
    {
        std::unordered_map<PK_id, BatchDescriptorSets>::iterator it = _batchDescriptorSets.find(batchIdentifier);

        if (it != _batchDescriptorSets.end())
        {
            BatchDescriptorSets& sets = it->second;

            const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
            const uint32_t swapchainImages = pSwapchain->getImageCount();
            std::vector<DescriptorSet*>& descriptorSet = sets._descriptorSet;

            if (descriptorSet.size() > 0)
            {
                Debug::log(
                    "@BatchContainer::createDescriptorSets "
                    "Attempted to create duplicate descriptor sets for batch: " + std::to_string(batchIdentifier),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }

            descriptorSet.resize(swapchainImages);

            for (uint32_t i = 0; i < swapchainImages; ++i)
            {
                std::vector<const Buffer*> b;
                if (!ubo.empty())
                    b.push_back(ubo[i]);
                DescriptorSet* pDescriptorSet = new DescriptorSet(
                    *pDescriptorSetLayout,
                    1,
                    textures,
                    b
                );
                descriptorSet[i] = pDescriptorSet;
            }

            Debug::log(
                "@Batch::createDescriptorSets "
                "Created " + std::to_string(descriptorSet.size()) + " "
                "descriptor sets successfully"
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
            for (DescriptorSet* d : it->second._descriptorSet)
                delete d;
            it->second._descriptorSet.clear();
        }
        Debug::log("@BatchContainer::freeDescriptorSets Descriptor sets freed");
    }

    void BatchContainer::clear()
    {
        for (Batch* pBatch : _batches)
            pBatch->clear();
    }

    bool BatchContainer::hasDescriptorSets(PK_id batchIdentifier) const
    {
        std::unordered_map<PK_id, BatchDescriptorSets>::const_iterator it = _batchDescriptorSets.find(batchIdentifier);
        if (it != _batchDescriptorSets.end())
            return !it->second._descriptorSet.empty();
        return false;
    }

    const DescriptorSet* BatchContainer::getDescriptorSet(PK_id batchIdentifier, uint32_t index) const
    {
        std::unordered_map<PK_id, BatchDescriptorSets>::const_iterator it = _batchDescriptorSets.find(batchIdentifier);
        if (it != _batchDescriptorSets.end())
        {
            const std::vector<DescriptorSet*>& sets = it->second._descriptorSet;
            if (index < sets.size())
            {
                return sets[index];
            }
            else
            {
                Debug::log(
                    "@BatchContainer::getDescriptorSet "
                    "Index: " + std::to_string(index) + " out of bounds! "
                    "descriptor sets length: " + std::to_string(sets.size()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
        }
        Debug::log(
            "@BatchContainer::getDescriptorSet "
            "No descriptor sets found for identifier: " + std::to_string(batchIdentifier),
            Debug::MessageType::PK_FATAL_ERROR
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
        freeDescriptorSets();
        initPipeline();
    }
}
