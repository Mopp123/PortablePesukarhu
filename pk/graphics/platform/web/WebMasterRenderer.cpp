
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

		void WebMasterRenderer::submit(const Component * const c)
		{}

		void WebMasterRenderer::render(mat4& projectionMatrix, mat4& viewMatrix)
		{}


		void WebMasterRenderer::resize(int w, int h)
		{
			glViewport(0, 0, w, h);
		}


		void WebMasterRenderer::beginRenderPass() 
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0, 0, 1, 1);
		}

		void WebMasterRenderer::endRenderPass()
		{

		}
	}
}