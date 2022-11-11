
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

		void WebMasterRenderer::submit(const Component * const c, const mat4& transformation)
		{}

		void WebMasterRenderer::render(const Camera& cam)
		{}

		void WebMasterRenderer::resize(int w, int h)
		{
			glViewport(0, 0, w, h);
			glFrontFace(GL_CCW);
		}


		void WebMasterRenderer::beginRenderPass() 
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.0f, 0.0f, 0.0f, 1);
		}

		void WebMasterRenderer::endRenderPass()
		{

		}
	}
}
