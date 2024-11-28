#pragma once

#include "ecs/components/Component.h"
#include "ecs/components/Camera.h"
#include "shaders/Shader.h"
#include "utils/pkmath.h"

#include "Swapchain.h"
#include "Buffers.h"
#include "resources/Texture.h"
#include "Pipeline.h"
#include "CommandBuffer.h"
#include "utils/ID.h"
#include <math.h>
#include <vector>
#include <unordered_map>

// TODO: Swapchain class


namespace pk
{
    // TODO:
    //  * Force command buffers to be "inside render pass"
    //  * RenderPass class
    enum RenderPassType
    {
        // atm used for everything..
        RENDER_PASS_DIFFUSE
    };


    class Batch
    {
    private:
        Buffer* _pBuffer = nullptr;
        size_t _writePos = 0;
        size_t _maxSize = 0;
        size_t _occupiedSize = 0;
        bool _occupied = false;
        PK_id _identifier = 0;
        uint32_t _instanceCount = 0;

    public:
        Batch(size_t maxSize);
        Batch(const Batch&) = delete;
        ~Batch();
        void occupy(PK_id identifier);
        bool addData(const void* pData, size_t dataSize);
        void clear();

        inline bool isOccupied() const { return _occupied; }
        inline bool isFull() const { return _occupiedSize == _maxSize; }
        inline size_t getOccupiedSize() const { return _occupiedSize; }
        inline Buffer* getBuffer() { return _pBuffer; }
        inline PK_id getIdentifier() const { return _identifier;}
        inline uint32_t getInstanceCount() const { return _instanceCount; }
    };

    class BatchContainer
    {
    private:
        std::vector<Batch*> _batches;

        struct BatchDescriptorSets
        {
            std::vector<DescriptorSet*> _descriptorSet;
        };

        // used kind of like descriptor set cache or smthn..
        std::unordered_map<PK_id, BatchDescriptorSets> _batchDescriptorSets;

    public:
        BatchContainer(size_t maxBatches, size_t batchSize);
        ~BatchContainer();
        // NOTE: Doesnt work when occupying new batch if batch exists with inputted
        // batchIdentifier
        // Returns true if added successfully
        //
        // NOTE: ubos needs to be given for each swapchain image, but not textures!
        bool addData(const void* data, size_t dataSize, PK_id batchIdentifier);
        void createDescriptorSets(
            PK_id batchIdentifier,
            const DescriptorSetLayout * const pDescriptorSetLayout = nullptr,
            const std::vector<const Texture*>& textures = {},
            const std::vector<const Buffer*>& ubo = {}
        );
        void freeDescriptorSets();
        void clear();

        inline std::vector<Batch*>& getBatches() { return _batches; }

        bool hasDescriptorSets(PK_id batchIdentifier) const;
        const DescriptorSet* getDescriptorSet(PK_id batchIdentifier, uint32_t index) const;
    };


    class MasterRenderer;
    class Renderer
    {
    protected:
        friend class MasterRenderer;

        Pipeline* _pPipeline = nullptr;
        std::unordered_map<RenderPassType, std::vector<CommandBuffer*>> _pCommandBuffers;

    public:
        Renderer();
        virtual ~Renderer();
        void handleWindowResize();

        // submit renderable component for rendering (batch preparing, before rendering)
        virtual void submit(
            const Component* const renderableComponent,
            const mat4& transformation,
            void* pCustomData = nullptr,
            size_t customDataSize = 0
        ) = 0;

        virtual void render() = 0;

        virtual void beginFrame() {}
        virtual void beginRenderPass(){}
        virtual void endRenderPass() {}
        virtual void endFrame() {}

        // Eventually this should do stuff like submitting cmd bufs to queues, etc.
        // atm only used to clear _batchContainers
        // (because those containers get filled each frame)
        virtual void flush() {}

        virtual void freeDescriptorSets() {}

        virtual void onSceneSwitch() {}

    protected:
        // Pipeline has its' own init func but what that func takes in dependant on renderer
        // so thats why we need this implemented for all renderers
        virtual void initPipeline() {}
    };

}
