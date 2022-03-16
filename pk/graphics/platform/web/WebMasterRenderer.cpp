
#include "WebMasterRenderer.h"
#include <GL/glew.h>

#include "WebBuffers.h"
#include "shaders/WebShader.h"
#include "shaders/sources/WebTestShader.h"

namespace pk
{
	namespace web
	{

		// JUST FOR TESTING

		WebShader* testShader = nullptr;
		WebVertexBuffer* vb = nullptr;
		WebIndexBuffer* ib = nullptr;


		WebMasterRenderer::WebMasterRenderer()
		{
			WebTestShader testShaderSource;
			testShader = new WebShader(testShaderSource.vertexSource, testShaderSource.fragmentSource);


			std::vector<PK_float> vertexData = {
				-0.5f, 0.5f,
				-0.5f, -0.5f,
				0.5f, -0.5f
			};

			std::vector<PK_ushort> indices = {
				0,1,2
			};


			vb = new WebVertexBuffer(vertexData, VertexBufferUsage::PK_BUFFER_USAGE_STATIC);
			ib = new WebIndexBuffer(indices);
		}

		WebMasterRenderer::~WebMasterRenderer()
		{
			delete vb;
			delete testShader;
		}

		void WebMasterRenderer::render()
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0, 0, 1, 1);

			glUseProgram(testShader->getProgramID());

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vb->getID());

			glVertexAttribPointer(0, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, sizeof(PK_float) * 2, 0);


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getID());


			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

			glDisableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
		}


		void WebMasterRenderer::resize(int w, int h)
		{
			glViewport(0, 0, w, h);
		}
	}
}