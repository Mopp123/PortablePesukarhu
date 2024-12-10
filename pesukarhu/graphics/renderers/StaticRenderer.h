#pragma once

#include "pesukarhu/resources/Texture.h"
#include "pesukarhu/resources/Mesh.h"
#include "pesukarhu/graphics/Renderer.h"
#include "pesukarhu/graphics/Pipeline.h"
#include "pesukarhu/graphics/Buffers.h"
#include "pesukarhu/graphics/Descriptors.h"
#include "pesukarhu/graphics/CommandBuffer.h"
#include "pesukarhu/graphics/RenderCommand.h"
#include "pesukarhu/graphics/shaders/Shader.h"

#include <unordered_map>


namespace pk
{
    class StaticRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        VertexBufferLayout _vertexBufferLayout;
        VertexBufferLayout _instanceBufferLayout;

        // TODO: Requires recreating if swapchain img count changes!
        std::vector<Buffer*> _materialDataUniformBuffers;
        std::vector<const Buffer*> _constMaterialDataUniformBuffers; // Needed cuz BatchContainer takes uniform buffers as const.. dumb as fuck I know..
        DescriptorSetLayout _materialDescSetLayout;

        //std::unordered_map<PK_id, Mesh*> _batchMeshCache;
        std::unordered_map<PK_id, vec4[2]> _batchMaterialData;
        BatchContainer _batchContainer;

    public:
        StaticRenderer();
        ~StaticRenderer();

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
    };
}
