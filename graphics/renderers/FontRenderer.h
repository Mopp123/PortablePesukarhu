#pragma once

#include "graphics/Renderer.h"
#include "graphics/Pipeline.h"
#include "graphics/Buffers.h"
#include "graphics/Texture.h"
#include "graphics/Descriptors.h"
#include "graphics/CommandBuffer.h"
#include "graphics/RenderCommand.h"
#include "graphics/shaders/Shader.h"
#include "utils/Font.h"
#include <unordered_map>


namespace pk
{
    struct FontPushConstants
    {
        mat4 projectionMatrix;
        float atlasRows = 0.0f;
    };

    class FontRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        Buffer* _pVertexBuffer = nullptr;
        Buffer* _pIndexBuffer = nullptr;
        VertexBufferLayout _vertexBufferLayout;

        VertexBufferLayout _instanceBufferLayout;

        DescriptorSetLayout _textureDescSetLayout;

        BatchContainer _batchContainer;

    public:
        FontRenderer();
        ~FontRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transform);
        virtual void render(const Camera& cam);

        virtual void flush();

        virtual void freeDescriptorSets();

    protected:
        virtual void initPipeline();
    };
}
