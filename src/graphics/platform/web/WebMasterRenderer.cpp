
#include "WebMasterRenderer.h"

#include <GL/glew.h>

namespace pk
{
	namespace web
	{

		WebMasterRenderer::WebMasterRenderer()
		{

		}

		WebMasterRenderer::~WebMasterRenderer()
		{

		}

		void WebMasterRenderer::render()
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0, 1, 0, 1);
		}
	}
}