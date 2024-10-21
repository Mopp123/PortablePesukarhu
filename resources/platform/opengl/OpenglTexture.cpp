#include "GL/glew.h"
#include "OpenglTexture.h"
#include "core/Debug.h"
#include "core/Application.h"
#include "resources/ResourceManager.h"


namespace pk
{
    namespace opengl
    {
        OpenglTexture::~OpenglTexture()
        {
            glDeleteTextures(1, &_glTexID);
        }

        OpenglTexture::OpenglTexture(
            uint32_t imgResourceID,
            TextureSampler sampler
        ) :
            Texture(sampler, imgResourceID)
        {
            Application* pApp = Application::get();
            if (!pApp)
            {
                Debug::log(
                    "@OpenglTexture::OpenglTexture Application was nullptr",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            ResourceManager& resourceManager = Application::get()->getResourceManager();
            const ImageData* pImgData = (const ImageData*)resourceManager.getResource(_imgResourceID);

            if (!pImgData)
            {
                Debug::log(
                    "Attempted to create OpenglTexture without providing image data. "
                    "This is currently illegal!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            if (!pImgData->getData())
            {
                Debug::log(
                    "Attempted to create OpenglTexture with invalid image data. "
                    "(image data's _pData is null)",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }

            GLint glFormat = 0;
            const int channels = pImgData->getChannels();
            const int width = pImgData->getWidth();
            const int height = pImgData->getHeight();
            _width = width;
            _height = height;
            _channels = channels;

            switch (channels)
            {
                case 1: glFormat = GL_ALPHA; break;
                case 3: glFormat = GL_RGB; break;
                case 4: glFormat = GL_RGBA; break;
                // NOTE: Noticed that atm only rgba works on web..
                default:
                    Debug::log(
                        "Invalid color channel count: " + std::to_string(channels) + " "
                        "while creating OpenglTexture. "
                        "Currently 4 channels are required (for some reason doesn't work on web if no 4 channels)",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    break;
            }

            glGenTextures(1, &_glTexID);
            glBindTexture(GL_TEXTURE_2D, _glTexID);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                glFormat,
                width,
                height,
                0,
                glFormat,
                GL_UNSIGNED_BYTE,
                (const void*)pImgData->getData()
            );

            // Address mode
            switch (_sampler.getAddressMode())
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
                    Debug::log(
                        "Invalid sampler address mode while trying to create OpenglTexture",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    break;
            }

            if (_sampler.getMipLevelCount() > 1)
            {
                if(_sampler.getFilterMode() == TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_LINEAR)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                }
                else if (_sampler.getFilterMode() == TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_NEAR)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                }

                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                if (_sampler.getFilterMode() == TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_LINEAR)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                }
                else if (_sampler.getFilterMode() == TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_NEAR)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                }
            }
            Debug::log("OpenglTexture texture created successfully");
            // NOTE: Not sure why not previously unbinding the texture here??
            glBindTexture(GL_TEXTURE_2D, 0);
        }


        void OpenglTexture::update(
            void* pData,
            size_t dataSize,
            size_t width,
            size_t height,
            uint32_t glTextureUnit
        )
        {
            if (width != _width)
            {
                Debug::log(
                    "@OpenglTexture::update "
                    "updated width: " + std::to_string(width) + " "
                    "was different from original: " + std::to_string(_width),
                    Debug::MessageType::PK_WARNING
                );
            }

            if (height != _height)
            {
                Debug::log(
                    "@OpenglTexture::update "
                    "updated height: " + std::to_string(height) + " "
                    "was different from original: " + std::to_string(_height),
                    Debug::MessageType::PK_WARNING
                );
            }

            _width = width;
            _height = height;

            GLint glFormat;
            switch (_channels)
            {
                case 1: glFormat = GL_ALPHA; break;
                case 3: glFormat = GL_RGB; break;
                case 4: glFormat = GL_RGBA; break;
                // NOTE: Noticed that atm only rgba works on web..
                default:
                    Debug::log(
                        "@OpenglTexture::update "
                        "Invalid color channel count: " + std::to_string(_channels) + " ",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    break;
            }

            switch (glTextureUnit)
            {
                case 0: glActiveTexture(GL_TEXTURE0); break;
                case 1: glActiveTexture(GL_TEXTURE1); break;
                case 2: glActiveTexture(GL_TEXTURE2); break;
                case 3: glActiveTexture(GL_TEXTURE3); break;
                case 4: glActiveTexture(GL_TEXTURE4); break;
                case 5: glActiveTexture(GL_TEXTURE5); break;
                default: break;
            }
            glBindTexture(GL_TEXTURE_2D, _glTexID);
            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0,
                0,
                width,
                height,
                glFormat,
                GL_UNSIGNED_BYTE,
                pData
            );
        }
    }
}
