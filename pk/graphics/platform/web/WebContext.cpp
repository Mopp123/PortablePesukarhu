
#include "WebContext.h"

#include <emscripten.h>
#include <emscripten/html5.h>

#include <GL/glew.h>

#include "../../../core/Debug.h"

namespace pk
{
	namespace web
	{
		WebContext::WebContext()
		{
			EmscriptenWebGLContextAttributes contextAttribs;
			emscripten_webgl_init_context_attributes(&contextAttribs);
			// ..propably the most disgusting line ever...
			contextAttribs.alpha = contextAttribs.depth = contextAttribs.stencil = contextAttribs.antialias = contextAttribs.preserveDrawingBuffer = contextAttribs.failIfMajorPerformanceCaveat = 0;
			contextAttribs.premultipliedAlpha = 0;
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
					Debug::log("Context(Web) created successfully");
				else
					Debug::log("Failed to init glew", Debug::MessageType::PK_FATAL_ERROR);
			}
			else
			{
				Debug::log("Failed to create WebGL context", Debug::MessageType::PK_FATAL_ERROR);
			}
		}

		WebContext::~WebContext()
		{}

	}
}