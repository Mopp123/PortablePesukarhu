#pragma once

#include <GL/glew.h>

#include "resources/Texture.h"

#include <string>

namespace pk
{
	namespace web
	{

		class WebTexture : public Texture
		{
		private:

			 GLuint _id = 0;


		public:

			WebTexture(const std::string& file, const TextureSampler& sampler, int tiling = 1);
			WebTexture(void* data, int width, int height, int channels, const TextureSampler& sampler);
			WebTexture(const WebTexture& other) = delete;

			~WebTexture();

			virtual void update(void* data, int slot = 0);

			inline GLuint getID() const { return _id; }
		};
	}
}
