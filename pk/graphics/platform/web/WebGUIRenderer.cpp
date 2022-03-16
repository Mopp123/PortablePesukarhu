
#include "WebGUIRenderer.h"
#include "shaders/sources/WebTestShader.h"

#include "../../../core/Debug.h"

namespace pk
{
	namespace web
	{

		static std::string s_vertexSource = R"(
                precision mediump float;
                
                attribute vec2 position;
                
                void main()
                {
                   gl_Position = vec4(position, 0, 1.0);
                }
            )";

		static std::string s_fragmentSource = R"(
                precision mediump float;
                
                void main()
                {
                    gl_FragColor = vec4(0,1,0,1);
                }
            )";

		
		WebVertexBuffer* vb = nullptr;
		WebIndexBuffer* ib = nullptr;

		WebGUIRenderer::WebGUIRenderer() : 
			_shader(s_vertexSource, s_fragmentSource)
		{

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
		
		WebGUIRenderer::~WebGUIRenderer()
		{
			delete vb;
			delete ib;
		}

		// submit renderable component for rendering (batch preparing, before rendering)
		void WebGUIRenderer::submit(const Component* const renderableComponent)
		{
			Debug::log("Submitting to WebGUIRenderer !");
		}

		void WebGUIRenderer::render()
		{
			
			glUseProgram(_shader.getProgramID());

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vb->getID());

			glVertexAttribPointer(0, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, sizeof(PK_float) * 2, 0);


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getID());


			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

			glDisableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

	}
}