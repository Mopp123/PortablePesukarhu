#pragma once

#include "resources/Texture.h"
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
    class SkinnedRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        VertexBufferLayout _vertexBufferLayout;
        VertexBufferLayout _instanceBufferLayout;

        // TODO: May need recreating if swapchain img count changes!
        std::vector<Buffer*> _materialPropsUniformBuffers;
        std::vector<const Buffer*> _constMaterialPropsUniformBuffers;
        DescriptorSetLayout _materialDescSetLayout;

        //std::unordered_map<PK_id, Mesh*> _batchMeshCache;
        std::unordered_map<PK_id, vec4> _batchMaterialProperties;
        BatchContainer _batchContainer;

    public:
        SkinnedRenderer();
        ~SkinnedRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transformation);
        virtual void render();

        virtual void flush();

        virtual void freeDescriptorSets();

    protected:
        virtual void initPipeline();
    };
}
