#pragma once

#include "pesukarhu/graphics/Renderer.h"
#include "pesukarhu/graphics/Pipeline.h"
#include "pesukarhu/graphics/Buffers.h"
#include "pesukarhu/graphics/Descriptors.h"
#include "pesukarhu/graphics/CommandBuffer.h"
#include "pesukarhu/graphics/RenderCommand.h"
#include "pesukarhu/graphics/shaders/Shader.h"
#include "pesukarhu/resources/Texture.h"
#include "pesukarhu/resources/Font.h"
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
