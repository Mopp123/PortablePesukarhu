#include "Texture.h"
#include "core/Debug.h"
#include "Context.h"
#include "platform/opengl/OpenglTexture.h"


namespace pk
{

    Texture_new::Texture_new(TextureSampler sampler, ImageData* pImgData, int tiling) :
        _sampler(sampler),
        _pImgData(pImgData),
        _tiling(tiling)
    {}

    Texture_new::~Texture_new()
    {
        if (_pImgData)
            delete _pImgData;
    }

    Texture_new* Texture_new::create(
        TextureSampler sampler,
        ImageData* pImgData,
        int tiling,
        bool saveDataHostSide
    )
    {
        const uint32_t api = Context::get_api_type();
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
                return new opengl::OpenglTexture(sampler, pImgData, tiling, saveDataHostSide);
            default:
                Debug::log(
                    "Attempted to create Texture but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }


    TextureAtlas::TextureAtlas(
        ImageData* imgdata,
        int tileCount,
        TextureSampler textureSampler
    ) :
        _tileCount(tileCount)
    {
        _texture = Texture_new::create(textureSampler, imgdata);
    }

    TextureAtlas::~TextureAtlas()
    {
        delete _texture;
    }
}
