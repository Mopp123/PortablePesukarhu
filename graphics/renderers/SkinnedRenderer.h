#pragma once

#include "resources/Texture.h"
#include "resources/Model.h"
#include "resources/Mesh.h"
#include "graphics/Renderer.h"
#include "graphics/Pipeline.h"
#include "graphics/Buffers.h"
#include "graphics/Descriptors.h"
#include "graphics/CommandBuffer.h"
#include "graphics/RenderCommand.h"
#include "graphics/shaders/Shader.h"

#include <unordered_map>


namespace pk
{
    class SkinnedMeshBatch
    {
    public:
        const Material* pMaterial = nullptr;
        vec4 materialProperties;
        std::vector<mat4> transformationMatrices;
        std::vector<entityID_t> rootJoints;
        size_t initialSize = 0;
        size_t occupiedCount = 0;
        std::vector<DescriptorSet*> materialDescriptorSet;

        // TODO: Make it possible on "client side project" to set initial
        // size on "init/scene switch stage"
        SkinnedMeshBatch(
            const Material* pMaterial,
            const vec4& materialProperties,
            size_t initialSize,
            const DescriptorSetLayout& materialDescriptorSetLayout,
            // NOTE: using same ubo for all batches
            //  ->we only change the data of the ubo to match batch when we render the batch
            const std::vector<const Buffer*>& pMaterialPropsUniformBuffers
        );
        SkinnedMeshBatch(const SkinnedMeshBatch& other) = delete;
        ~SkinnedMeshBatch();

        void add(const mat4& transformationMatrix, entityID_t rootJoint);

        // NOTE: We dont clear transformationMatrices here -> we only zero it
        // because we can be quite sure that its' size remains the same on
        // next frame as well
        void clear();
    };


    class SkinnedRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        VertexBufferLayout _vertexBufferLayout;

        // TODO: May need recreating if swapchain img count changes!
        std::vector<Buffer*> _materialPropsUniformBuffers;
        std::vector<const Buffer*> _constMaterialPropsUniformBuffers;
        DescriptorSetLayout _materialDescSetLayout;

        std::vector<Buffer*> _jointMatricesBuffer;
        DescriptorSetLayout _jointDescriptorSetLayout;
        std::vector<DescriptorSet*> _jointDescriptorSet;

        // How batching works here?
        // -> If no batch exists for submitted renderable
        //  -> create new batch (creates required descriptor sets too)
        //  -> we add new entry to _identifierBatchMapping using the renderable's
        //  mesh's resource id as "identifier"
        //
        //  -> on flush() we only clear the _identifierBatchMapping and NOT the created
        //  batch objects
        //      -> already created batches are ready to be used through out the whole scene's life
        //      -> the actual _batches gets destroyed on freeDescriptorSets() which happens
        //      on swapchain recreation(requires creating new descriptor sets) and scene switch
        std::vector<SkinnedMeshBatch*> _batches;
        std::unordered_map<PK_id, int> _identifierBatchMapping;
        std::unordered_map<PK_id, Mesh*> _batchMeshMapping;

        // Just temp hack to get this working..
        // TODO: Some better solution to this!
        std::unordered_map<PK_id, PK_id> _batchModelIDMapping;
        std::unordered_map<PK_id, Model*> _batchModelMapping;

    public:
        SkinnedRenderer();
        ~SkinnedRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transformation);
        virtual void render();

        virtual void flush();

        virtual void freeDescriptorSets();

    protected:
        virtual void initPipeline();

    private:
        void createJointUniformBuffers(size_t swapchainImageCount);
        void createJointDescriptorSets(const std::vector<Buffer*>& jointBuffers);
    };
}
