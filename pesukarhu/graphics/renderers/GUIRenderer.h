#pragma once

#include "pesukarhu/graphics/Renderer.h"
#include "pesukarhu/graphics/Pipeline.h"
#include "pesukarhu/graphics/Buffers.h"
#include "pesukarhu/graphics/Descriptors.h"
#include "pesukarhu/graphics/CommandBuffer.h"
#include "pesukarhu/graphics/RenderCommand.h"
#include "pesukarhu/graphics/shaders/Shader.h"
#include "pesukarhu/resources/Texture.h"

#include <unordered_map>


namespace pk
{
    class GUIRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        Buffer* _pVertexBuffer = nullptr;
        Buffer* _pIndexBuffer = nullptr;
        VertexBufferLayout _vertexBufferLayout;

        VertexBufferLayout _instanceBufferLayout;

        DescriptorSetLayout _textureDescSetLayout;

        /*
        struct BatchDescriptorSets
        {
            std::vector<DescriptorSet*> pTextureDescriptorSet;
        };

        std::unordered_map<PK_id, BatchDescriptorSets> _descriptorSets;
        */
        // Testing new Batch system..
        BatchContainer _batchContainer;

    public:
        GUIRenderer();
        ~GUIRenderer();

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
