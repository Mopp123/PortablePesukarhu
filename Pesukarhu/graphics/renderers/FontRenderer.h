#pragma once

#include "Pesukarhu/graphics/Renderer.h"
#include "Pesukarhu/graphics/Pipeline.h"
#include "Pesukarhu/graphics/Buffers.h"
#include "Pesukarhu/graphics/Descriptors.h"
#include "Pesukarhu/graphics/CommandBuffer.h"
#include "Pesukarhu/graphics/RenderCommand.h"
#include "Pesukarhu/graphics/shaders/Shader.h"
#include "Pesukarhu/resources/Texture.h"
#include "Pesukarhu/resources/Font.h"
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
