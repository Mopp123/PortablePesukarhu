#pragma once

#include "ecs/systems/System.h"
#include "graphics/Pipeline.h"
#include "graphics/Buffers.h"
#include "graphics/Texture.h"
#include "graphics/Descriptors.h"
#include "graphics/CommandBuffer.h"
#include "graphics/RenderCommand.h"
#include "graphics/shaders/Shader.h"


namespace pk
{
    class GUIRenderer : public System
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;
        Pipeline* _pPipeline = nullptr;

        CommandBuffer* _pCmdBuf = nullptr;

        RenderCommand* _pRenderCommand = nullptr;

        // NOTE: atm these here only for testing!
        Buffer* _pVertexBuffer = nullptr;
        Buffer* _pIndexBuffer = nullptr;

        Buffer* _pTestUBO = nullptr;
        Texture_new* _pTestTexture = nullptr;

        DescriptorSet* _pUBODescriptorSet = nullptr;
        DescriptorSet* _pTextureDescriptorSet = nullptr;

    public:
        GUIRenderer();
        ~GUIRenderer();

        virtual void update();
    };
}
