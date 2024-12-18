#include "Texture.h"
#include "pesukarhu/core/Debug.h"
#include "pesukarhu/graphics/Context.h"
#include "pesukarhu/core/Application.h"
#include "platform/opengl/OpenglTexture.h"


namespace pk
{
    Texture::Texture(TextureSampler sampler, uint32_t imgResourceID) :
        Resource(ResourceType::RESOURCE_TEXTURE),
        _sampler(sampler),
        _imgResourceID(imgResourceID)
    {}

    Texture::~Texture()
    {}

    // TODO: Be able to call this only through ResourceManager
    Texture* Texture::create(
        uint32_t imgResourceID,
        TextureSampler sampler
    )
    {
        const GraphicsAPI api = Context::get_graphics_api();
        switch(api)
        {
            case GraphicsAPI::PK_GRAPHICS_API_WEBGL:
            {
                return new opengl::OpenglTexture(imgResourceID, sampler);
            }
            default:
                Debug::log(
                    "Attempted to create Texture but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }


    TextureAtlas::TextureAtlas(
        uint32_t textureResourceID,
        int rowCount
    ) :
        _textureResourceID(textureResourceID),
        _rowCount(rowCount)
    {}

    TextureAtlas::~TextureAtlas()
    {}

    /*
    const Texture_new * const TextureAtlas::getTexture() const
    {
        // TODO: Better way of accessing application/resource manager
        // the way u dont always need to make sure Application exists..
        // -> or make it a singleton or smthn...
        Application* pApp = Application::get();
        if (!pApp)
        {
            Debug::log(
                "@TextureAtlas::getTexture Application was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        return pApp->getResourceManager().getTexture(_textureResourceID);
    }*/
}
