#pragma once

#include "CommandBuffer.h"


namespace pk
{
    class RenderCommand
    {
    public:
        virtual ~RenderCommand() {}
        // TODO: eventually this should return "primary command buffer" to use for currently used frame
        virtual void beginFrame() = 0;
        virtual void beginRenderPass() = 0;
        virtual void endRenderPass() = 0;
        virtual void endFrame() = 0;

        // NOTE: atm just quick hack and only opengl specific!!!
        virtual void resizeViewport(int width, int height) = 0;

        static RenderCommand* create();
    };
}
