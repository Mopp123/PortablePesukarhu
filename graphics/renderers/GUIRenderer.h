#pragma once

#include "graphics/Renderer.h"
#include "graphics/Pipeline.h"
#include "graphics/Buffers.h"
#include "graphics/Texture.h"
#include "graphics/Descriptors.h"
#include "graphics/CommandBuffer.h"
#include "graphics/RenderCommand.h"
#include "graphics/shaders/Shader.h"

#include <unordered_map>


namespace pk
{
    class GUIRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        // NOTE: atm these here only for testing!
        Buffer* _pVertexBuffer = nullptr;
        Buffer* _pIndexBuffer = nullptr;
        VertexBufferLayout _vertexBufferLayout;

        // All per instance data
        Buffer* _pInstanceBuffer = nullptr;
        VertexBufferLayout _instanceBufferLayout;
        size_t _instanceCount = 0;
        size_t _instanceBufWritePos = 0;
        size_t _instanceBufElemLen = 4; // pos + scale = 4 floats

        DescriptorSetLayout _textureDescSetLayout;

        std::unordered_map<Texture_new*, std::vector<DescriptorSet*>> _textureDescriptorSets;

        // Testing new Batch system..
        BatchContainer _batchContainer;

    public:
        GUIRenderer();
        ~GUIRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transformation);
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
