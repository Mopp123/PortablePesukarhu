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

#include "ecs/components/renderable/TerrainRenderable.h"

namespace pk
{
    class TerrainRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        VertexBufferLayout _vertexBufferLayout;

        // TODO: Requires recreating if swapchain img count changes!
        std::vector<Buffer*> _materialPropsUniformBuffers;
        DescriptorSetLayout _materialDescSetLayout;

        std::vector<const TerrainRenderable*> _toRender;

    public:
        TerrainRenderer();
        ~TerrainRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transformation);
        virtual void render();

        virtual void flush();

        virtual void freeDescriptorSets();

    protected:
        virtual void initPipeline();
    };
}
