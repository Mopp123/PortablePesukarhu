#pragma once

#include "../../Renderer.h"
#include "../../../ecs/components/renderable/GUIRenderable.h"
#include "WebBuffers.h"
#include "shaders/WebShader.h"

#include <vector>


namespace pk
{
	namespace web
	{
		class WebGUIRenderer : public Renderer
		{
		private:

			WebShader _shader;

		public:

			WebGUIRenderer();
			~WebGUIRenderer();

			// submit renderable component for rendering (batch preparing, before rendering)
			virtual void submit(const Component* const renderableComponent);

			virtual void render();

			virtual void resize(int w, int h) {}

		};
	}

}