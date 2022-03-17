#pragma once

#include "../ecs/components/Component.h"
#include "shaders/Shader.h"

#include <vector>

namespace pk
{

	class Renderer
	{
	public:

		Renderer() {}
		virtual ~Renderer() {}

		// submit renderable component for rendering (batch preparing, before rendering)
		virtual void submit(const Component* const renderableComponent) = 0;

		virtual void render() = 0;

		virtual void resize(int w, int h) = 0;

		virtual void beginFrame() {}
		virtual void beginRenderPass(){}
		virtual void endRenderPass() {}
		virtual void endFrame() {}

	};

}