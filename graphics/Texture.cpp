#include "Texture.h"
#include "core/Debug.h"
#include "Context.h"
#include "platform/opengl/OpenglTexture.h"


namespace pk
{
    Texture_new* Texture_new::create(TextureSampler sampler, ImageData* pImgData, int tiling)
    {
        const uint32_t api = Context::get_api_type();
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
                return new opengl::OpenglTexture(sampler, pImgData, tiling);
            default:
                Debug::log(
                    "Attempted to create Texture but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }

    }
}
