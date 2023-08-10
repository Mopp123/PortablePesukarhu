
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

		static GLuint create_GL_texture(void* data, int width, int height, int channels, const TextureSampler& sampler)
		{
			GLint glFormat = 0;
			switch (channels)
			{
			case 1: glFormat = GL_ALPHA;break;
			case 3: glFormat = GL_RGB;	break;
			case 4: glFormat = GL_RGBA; break;

			default:
				Debug::log("Invalid color channel count when loading texture", Debug::MessageType::PK_FATAL_ERROR);
				return 0;
			}

			GLuint texID = 0;

			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);

			// Address mode
			switch (sampler.getAddressMode())
			{
			case TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_REPEAT :
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			case TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
				// *It appears that webgl 1.0 doesnt have "GL_CLAMP_TO_BORDER"
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			default:
				break;
			}

			if (sampler.getMipLevelCount() > 1)
			{
				if(sampler.getFilterMode() == TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_LINEAR)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
				else if (sampler.getFilterMode() == TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_NEAR)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}

				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				if (sampler.getFilterMode() == TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_LINEAR)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
				else if (sampler.getFilterMode() == TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_NEAR)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}
			}

			Debug::log("GL texture created successfully");

			return texID;
		}

		static void update_GL_texture(void* data, int width, int height, WebTexture& texture, int slot)
		{
			GLint glFormat = 0;
			switch (texture.getChannels())
			{
			case 1: glFormat = GL_ALPHA; break;
			case 3: glFormat = GL_RGB;	break;
			case 4: glFormat = GL_RGBA; break;

			default:
				Debug::log("Invalid color channel count when loading texture", Debug::MessageType::PK_FATAL_ERROR);
				return;
			}
			switch (slot)
			{
				case 0: glActiveTexture(GL_TEXTURE0); break;
				case 1: glActiveTexture(GL_TEXTURE1); break;
				case 2: glActiveTexture(GL_TEXTURE2); break;
				case 3: glActiveTexture(GL_TEXTURE3); break;
				case 4: glActiveTexture(GL_TEXTURE4); break;
				case 5: glActiveTexture(GL_TEXTURE5); break;
				default: break;
			}
			glBindTexture(GL_TEXTURE_2D, texture.getID());
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, glFormat, GL_UNSIGNED_BYTE, data);
		}


		WebTexture::WebTexture(void* data, int width, int height, int channels, const TextureSampler& sampler) :
			Texture(sampler, width, height, channels)
		{
			_id = create_GL_texture(data, width, height, channels, sampler);
		}


		WebTexture::WebTexture(const std::string& filename, const TextureSampler& sampler, int tiling) :
			Texture(sampler, tiling)
		{
			SDL_Surface* surface = IMG_Load(filename.c_str());

			if (surface == NULL)
			{
				Debug::log(
                                    "Failed to create SDL surface from: " + filename + " SDL_image error: " + IMG_GetError(),
                                    Debug::MessageType::PK_FATAL_ERROR
                                );
				return;
			}
			_width = surface->w;
			_height = surface->h;
			_channels = 4;
			_id = create_GL_texture(surface->pixels, _width, _height, _channels, sampler);

			SDL_FreeSurface(surface);
		}


		WebTexture::~WebTexture()
		{
			Debug::log("Texture destroyed");
			glDeleteTextures(1, &_id);
		}


		void WebTexture::update(void* data, int slot)
		{
			update_GL_texture(data, _width, _height, *this, slot);
		}

	}
}
