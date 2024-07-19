#pragma once

#include "graphics/Renderer.h"
#include "graphics/Pipeline.h"
#include "graphics/Buffers.h"
#include "graphics/Texture.h"
#include "graphics/Mesh.h"
#include "graphics/Descriptors.h"
#include "graphics/CommandBuffer.h"
#include "graphics/RenderCommand.h"
#include "graphics/shaders/Shader.h"

#include <unordered_map>


namespace pk
{
    class StaticRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        Buffer* _pVertexBuffer = nullptr;
        Buffer* _pIndexBuffer = nullptr;
        VertexBufferLayout _vertexBufferLayout;

        // TODO: Figure out how to handle renderable specific shit like this..
        std::vector<Buffer*> _pTransformUBO;

        DescriptorSetLayout _UBODescSetLayout;
        DescriptorSetLayout _textureDescSetLayout;

        std::vector<DescriptorSet*> _pUBODescriptorSet;
        std::vector<DescriptorSet*> _pTextureDescriptorSet;

        Mesh* _pTestMesh = nullptr;

    public:
        StaticRenderer();
        ~StaticRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transformation);
        virtual void render(const Camera& cam);

        virtual void flush();

    protected:
        virtual void initPipeline();
        virtual void freeDescriptorSets();
    };
}
