#include "WebContext.h"

#ifdef PK_BUILD_WEB
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <GL/glew.h>

#include "../../../core/Debug.h"

namespace pk
{
    namespace web
    {
        WebContext::WebContext()
        {
        #ifdef PK_BUILD_WEB
            EmscriptenWebGLContextAttributes contextAttribs;
            emscripten_webgl_init_context_attributes(&contextAttribs);
            // ..propably the most disgusting line ever...
            contextAttribs.alpha = false;

            contextAttribs.depth = true;
            contextAttribs.stencil = false;
            contextAttribs.antialias = true;
            contextAttribs.premultipliedAlpha = true;
            contextAttribs.preserveDrawingBuffer = false;

            contextAttribs.failIfMajorPerformanceCaveat = false;

            contextAttribs.enableExtensionsByDefault = 1;

            // using webgl 1.0 (widest support?)
            contextAttribs.majorVersion = 1;
            contextAttribs.majorVersion = 0;

            EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webglContext = emscripten_webgl_create_context("#canvas", &contextAttribs);
            if (webglContext >= 0)
            {
                emscripten_webgl_make_context_current(webglContext);

                GLenum glewInitStatus = glewInit();
                if (glewInitStatus == GLEW_OK)
                {
                    // Query some limits...
                    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);
                    Debug::log(
                        "Context(Web) created successfully! "
                        "Available texture units: " + std::to_string(_maxTextureUnits)
                    );
                }
                else
                {
                    Debug::log("Failed to init glew", Debug::MessageType::PK_FATAL_ERROR);
                }
            }
            else
            {
                Debug::log("Failed to create WebGL context", Debug::MessageType::PK_FATAL_ERROR);
            }
        #endif
        }

        WebContext::~WebContext()
        {}
    }
}
