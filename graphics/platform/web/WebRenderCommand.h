#pragma once

#include "../../RenderCommand.h"


namespace pk
{
    namespace web
    {
        class WebRenderCommand : public RenderCommand
        {
        public:
            ~WebRenderCommand() {}
            virtual void beginFrame();
            virtual void beginRenderPass();
            virtual void endRenderPass();
            virtual void endFrame();

            // NOTE: atm just quick hack and only opengl specific!!!
            virtual void resizeViewport(int width, int height);
        };
    }
}
