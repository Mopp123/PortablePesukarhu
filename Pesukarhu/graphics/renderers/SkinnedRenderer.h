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
#include "ecs/components/AnimationData.h"

#include <unordered_map>


namespace pk
{
    // TODO: Maybe also have possibility of having version of batching which
    // uses less mem (causing it also to be slower..)
    class SkinnedMeshBatch
    {
    public:
        const Material* pMaterial = nullptr;
        // *stride = mat4 * maxJoints -> max joint matrices per entity
        std::vector<mat4> jointMatrices;
        std::vector<mat4> transformationMatrices; // transformation matrix per skinned renderable because joint matrices don't have world transform
        size_t initialCount = 0;
        size_t occupiedCount = 0;
        std::vector<DescriptorSet*> materialDescriptorSet;

        // TODO: Make it possible on "client side project" to set initial
        // size on "init/scene switch stage"
        SkinnedMeshBatch(
            const Material* pMaterial,
            size_t initialCount,
            const DescriptorSetLayout& materialDescriptorSetLayout,
            // NOTE: using same ubo for all batches
            //  ->we only change the data of the ubo to match batch when we render the batch
            const std::vector<const Buffer*>& pMaterialPropsUniformBuffers
        );
        SkinnedMeshBatch(const SkinnedMeshBatch& other) = delete;
        ~SkinnedMeshBatch();

        void add(
            const Scene * const pScene,
            const Mesh * const pMesh,
            const mat4& transformationMatrix,
            const AnimationData * const pAnimData
        );

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

    public:
        SkinnedRenderer();
        ~SkinnedRenderer();

        virtual void submit(
            const Component* const renderableComponent,
            const mat4& transformation,
            void* pCustomData = nullptr,
            size_t customDataSize = 0
        );
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
