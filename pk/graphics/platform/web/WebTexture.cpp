
#include <emscripten.h>
#include <GL/glew.h>
#include <SDL_image.h>

#include "WebTexture.h"

#include "../../../core/Debug.h"
#include "../../../Common.h"

#include <stdio.h>

namespace pk
{
	namespace web
	{

		static GLuint create_GL_texture(void* data, int width, int height, int channels = 4)
		{
			GLint glFormat = 0;
			switch (channels)
			{
			case 1: glFormat = GL_R; break;
			case 3: glFormat = GL_RGB; break;
			case 4: glFormat = GL_RGBA; break;
			
			default:
				Debug::log("Invalid color channel count when loading texture", Debug::MessageType::PK_FATAL_ERROR);
				return 0;
			}

			GLuint texID = 0;

			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			Debug::log("GL texture created successfully");

			return texID;
		}


		WebTexture::WebTexture(void* data, int width, int height, int channels)
		{
			_id = create_GL_texture(data, width, height, channels);
		}


		WebTexture::WebTexture(const std::string& filename)
		{
			SDL_Surface* surface = IMG_Load(filename.c_str());

			if (surface == NULL)
			{
				Debug::log("Failed to create SDL surface from: " + filename, Debug::MessageType::PK_FATAL_ERROR);
				return;
			}

			_id = create_GL_texture(surface->pixels, surface->w, surface->h);

			SDL_FreeSurface(surface);
		}



		WebTexture::~WebTexture()
		{
			Debug::log("Texture destroyed");
			glDeleteTextures(1, &_id);
		}

		
	}
}