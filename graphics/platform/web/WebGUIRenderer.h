#pragma once

#include "../../Renderer.h"
#include "../../../ecs/components/renderable/GUIRenderable.h"
#include "WebBuffers.h"
#include "shaders/WebShader.h"
#include "WebTexture.h"

#include "../../../core/Debug.h"

#include <vector>
#include <unordered_map>

namespace pk
{
	namespace web
	{
		
		class WebGUIRenderer : public Renderer
		{
		private:

			WebShader _shader;

			PK_int _vertexAttribLocation_pos = -1;
			PK_int _vertexAttribLocation_uv = -1;
			PK_int _vertexAttribLocation_color = -1;
			PK_int _vertexAttribLocation_properties = -1;

			PK_int _uniformLocation_projMat = -1;
			PK_int _uniformLocation_texSampler = -1;
			
			WebTexture* _defaultTexture = nullptr;

			std::vector<BatchData> _batches;

		public:

			WebGUIRenderer();
			~WebGUIRenderer();

			// submit renderable component for rendering (batch preparing, before rendering)
			virtual void submit(const Component* const renderableComponent, const mat4& transformation);

			virtual void render(const Camera& cam);

			virtual void resize(int w, int h) {}

		private:

			void allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength);
		};
	}

}
