#pragma once

#include "../ecs/components/Component.h"
#include "../ecs/components/Camera.h"
#include "shaders/Shader.h"
#include "../utils/pkmath.h"

#include "Swapchain.h"
#include "Buffers.h"
#include "Texture.h"
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

    // TODO: REDO / DELETE the whole BatchData shit!
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

        int ID = -1; // Identifier, how we find suitable batch for a renderable (can be anythin u want) (*NOTE! this is becomming quite obsolete after added 'Texture*' for this)
        bool isFree = true; // If theres nobody yet in this batch (completely empty -> available for occupying for new batch)(DON'T confuse with fullness:D)
        bool isFull = false;
        int currentDataPtr = 0; // When submitting to this batch, we use this to determine, at which position of the batch, we want to insert new data
        const Texture* texture = nullptr; // Optionally you can specify a texture which to use for this whole batch

        // *Ownerships of the buffers gets always transferred to this BatchData instance
        // *Ownership of the texture doesn't get transferred!
        BatchData(int dataEntryLen, int totalDataLen, const std::vector<VertexBuffer*>& vbs, IndexBuffer* ib, const Texture* tex = nullptr) :
            instanceDataLen(dataEntryLen), maxTotalBatchDataLen(totalDataLen), vertexBuffers(vbs), indexBuffer(ib), texture(tex)
        {}
        // *Ownerships gets always transferred to this BatchData
        BatchData(const BatchData& other) :
            instanceDataLen(other.instanceDataLen),
            maxTotalBatchDataLen(other.maxTotalBatchDataLen),
            vertexBuffers(other.vertexBuffers),
            indexBuffer(other.indexBuffer),
            texture(other.texture)
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
            texture = nullptr;
            isFree = true;
            isFull = false;
            currentDataPtr = 0;
        }

        inline int getInstanceCount() const { return currentDataPtr / instanceDataLen; }

        void occupy(int batchId, const Texture* tex = nullptr)
        {
            ID = batchId;
            texture = tex;
            isFree = false;
        }

        void addNewInstance();
        void insertInstanceData(int bufferIndex, const std::vector<float>& data);
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
        std::unordered_map<PK_id, Batch*> _occupiedBatches;

        struct BatchDescriptorSets
        {
            std::vector<DescriptorSet*> _textureDescriptorSet;
            std::vector<DescriptorSet*> _uboDescriptorSet;
        };

        // used kind of like descriptor set cache or smthn..
        std::unordered_map<PK_id, BatchDescriptorSets> _batchDescriptorSets;

    public:
        BatchContainer(size_t maxBatches, size_t batchSize);
        ~BatchContainer();
        // NOTE: Doesnt work when occupying new batch if batch exists with inputted
        // batchIdentifier
        // Returns true if added successfully
        bool addData(const void* data, size_t dataSize, PK_id batchIdentifier);
        void createDescriptorSets(
            PK_id batchIdentifier,
            const DescriptorSetLayout * const pTextureDescriptorSetLayout = nullptr,
            Texture_new* pTexture = nullptr,
            const DescriptorSetLayout * const pUboDescriptorSetLayout = nullptr,
            const std::vector<Buffer*>& ubo = {}
        );
        void freeDescriptorSets();
        void clear();

        const Batch* getBatch(PK_id batchIdentifier) const;
        inline Batch* accessBatch_DANGER(PK_id batchIdentifier) { return _occupiedBatches[batchIdentifier]; }
        inline std::vector<Batch*>& getBatches() { return _batches; }
        inline const std::unordered_map<PK_id, Batch*>& getOccupiedBatches() const { return _occupiedBatches; }
        inline std::unordered_map<PK_id, Batch*>& accessOccupiedBatches() { return _occupiedBatches; }

        bool hasDescriptorSets(PK_id batchIdentifier) const;
        const DescriptorSet* getTextureDescriptorSet(PK_id batchIdentifier, uint32_t index) const;
        const DescriptorSet* getUboDescriptorSet(PK_id batchIdentifier, uint32_t index) const;
    };


    class Renderer
    {
    protected:
        Pipeline* _pPipeline = nullptr;
        std::unordered_map<RenderPassType, std::vector<CommandBuffer*>> _pCommandBuffers;

    public:
        Renderer();
        virtual ~Renderer();
        void handleWindowResize();

        // submit renderable component for rendering (batch preparing, before rendering)
        virtual void submit(const Component* const renderableComponent, const mat4& transformation) = 0;

        virtual void render(const Camera& cam) = 0; //*why the fuk proj and view matrices not const?

        virtual void beginFrame() {}
        virtual void beginRenderPass(){}
        virtual void endRenderPass() {}
        virtual void endFrame() {}

        // Eventually this should do stuff like submitting cmd bufs to queues, etc.
        // atm only used to clear _batchContainers
        // (because those containers get filled each frame)
        virtual void flush() {}

        // Responsible for creating descriptor sets depending on the needs of the component.
        // On submit(...) should be checked does component have appropriate descriptor sets
        // already created. If no -> call this
        virtual void createDescriptorSets(Component* pComponent) {}
        virtual void freeDescriptorSets() {}

    protected:
        // Pipeline has its' own init func but what that func takes in dependant on renderer
        // so thats why we need this implemented for all renderers
        virtual void initPipeline() {}
    };

}
