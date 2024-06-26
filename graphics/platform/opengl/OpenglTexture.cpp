#include "GL/glew.h"
#include "OpenglTexture.h"
#include "core/Debug.h"


namespace pk
{
    namespace opengl
    {
        OpenglTexture::~OpenglTexture()
        {
            glDeleteTextures(1, &_id);
        }

        OpenglTexture::OpenglTexture(
            TextureSampler sampler,
            ImageData* pImgData,
            int tiling,
            bool saveDataHostSide
        ) :
            Texture_new(sampler, pImgData, tiling)
        {
            if (!_pImgData)
            {
                Debug::log(
                    "Attempted to create OpenglTexture without providing image data. "
                    "This is currently illegal!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            if (!_pImgData->getData())
            {
                Debug::log(
                    "Attempted to create OpenglTexture with invalid image data. "
                    "(image data's _pData is null)",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }

            GLint glFormat = 0;
            const int channels = _pImgData->getChannels();
            const int width = _pImgData->getWidth();
            const int height = _pImgData->getHeight();

            switch (channels)
            {
                case 1: glFormat = GL_ALPHA; break;
                case 3: glFormat = GL_RGB; break;
                case 4: glFormat = GL_RGBA; break;
                default:
                    Debug::log(
                        "Invalid color channel count while creating OpenglTexture",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    break;
            }

            glGenTextures(1, &_id);
            glBindTexture(GL_TEXTURE_2D, _id);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                glFormat,
                width,
                height,
                0,
                glFormat,
                GL_UNSIGNED_BYTE,
                (const void*)_pImgData->getData()
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

            if (!saveDataHostSide)
            {
                #ifdef PK_DEBUG_FULL
                Debug::log(
                    "WebBuffer not marked to be saved on host side. "
                    "Deleting host side buffer"
                );
                #endif
                delete _pImgData;
                _pImgData = nullptr;
            }
        }
    }
}
