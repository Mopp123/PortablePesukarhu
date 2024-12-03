#pragma once

#include "Pesukarhu/graphics/Renderer.h"
#include "Pesukarhu/graphics/Pipeline.h"
#include "Pesukarhu/graphics/Buffers.h"
#include "Pesukarhu/graphics/Descriptors.h"
#include "Pesukarhu/graphics/CommandBuffer.h"
#include "Pesukarhu/graphics/RenderCommand.h"
#include "Pesukarhu/graphics/shaders/Shader.h"
#include "Pesukarhu/resources/Texture.h"

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
