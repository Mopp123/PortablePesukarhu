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
    struct FontDetails
    {
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

        Buffer* _pInstanceBuffer = nullptr;
        VertexBufferLayout _instanceBufferLayout;

        DescriptorSetLayout _textureDescSetLayout;
        std::unordered_map<Texture_new*, std::vector<DescriptorSet*>> _textureDescriptorSet;

        // Font details ubo stuff..
        DescriptorSetLayout _detailsDescriptorSetLayout;
        std::vector<Buffer*> _pDetailsUniformBuffer;
        std::unordered_map<Font*, std::vector<DescriptorSet*>> _detailsDescriptorSet;

        BatchContainer _batchContainer;

        // Atm just testing here!
        // TODO: Put font id to TextRenderables and allow using different fonts
        Font* _pFont = nullptr;

    public:
        FontRenderer();
        ~FontRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transform);
        virtual void render(const Camera& cam);

        virtual void flush();

        virtual void createDescriptorSets(Component* pComponent);

    protected:
        virtual void initPipeline();
        virtual void freeDescriptorSets();
        virtual void recreateDescriptorSets();

    private:
        void clearInstances();
    };
}
