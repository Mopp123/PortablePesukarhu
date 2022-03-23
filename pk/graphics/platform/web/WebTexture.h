#pragma once

#include <GL/glew.h>

#include "../../Texture.h"

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

			WebTexture(const std::string& file);
			WebTexture(void* data, int width, int height, int channels);
			WebTexture(const WebTexture& other) = delete;
			
			~WebTexture();
			
			
			inline GLuint getID() const { return _id; }
		};
	}
}