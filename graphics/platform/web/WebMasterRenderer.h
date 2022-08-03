#pragma once

#include "../../Renderer.h"

namespace pk
{
	namespace web
	{

		class WebMasterRenderer : public Renderer
		{
		public:

			WebMasterRenderer();
			~WebMasterRenderer();

			virtual void submit(const Component * const c, const mat4& transformation);

			virtual void render(const Camera& cam);
			virtual void resize(int w, int h);


			virtual void beginRenderPass() override;
			virtual void endRenderPass() override;

		};
	}
}