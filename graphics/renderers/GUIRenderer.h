#pragma once

#include "graphics/Renderer.h"
#include "graphics/Pipeline.h"
#include "graphics/Buffers.h"
#include "graphics/Texture.h"
#include "graphics/Descriptors.h"
#include "graphics/CommandBuffer.h"
#include "graphics/RenderCommand.h"
#include "graphics/shaders/Shader.h"


namespace pk
{
    class GUIRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        // NOTE: atm these here only for testing!
        Buffer* _pVertexBuffer = nullptr;
        Buffer* _pInstancedVertexBuffer = nullptr;
        Buffer* _pIndexBuffer = nullptr;

        DescriptorSetLayout _textureDescSetLayout;
        Texture_new* _pTestTexture = nullptr;
        Texture_new* _pTestTexture2 = nullptr;

        DescriptorSet* _pTextureDescriptorSet = nullptr;

        float s_testX = 0.0f;

    public:
        GUIRenderer();
        ~GUIRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transformation);
        virtual void render(const Camera& cam);

    protected:
        virtual void initPipeline();
    };
}
