#include "WebRenderCommand.h"
#include <GL/glew.h>


namespace pk
{
    namespace web
    {
        void WebRenderCommand::beginFrame()
        {}

        void WebRenderCommand::beginRenderPass()
        {
            // NOTE: Not sure in which order these should be done..


	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    glClearColor(0.0f, 0.0f, 0.0f, 1);
        }

        void WebRenderCommand::endRenderPass()
        {}

        void WebRenderCommand::endFrame()
        {}


        // NOTE: atm just quick hack and only opengl specific!!!
        void WebRenderCommand::resizeViewport(int width, int height)
        {
            glViewport(0, 0, width, height);
            glFrontFace(GL_CCW); // Not sure why this is here?
        }
    }
}
