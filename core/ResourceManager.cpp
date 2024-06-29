#include "ResourceManager.h"
#include "core/Debug.h"
#include "utils/ID.h"


namespace pk
{
    ResourceManager::ResourceManager()
    {}

    ResourceManager::~ResourceManager()
    {}

    ImageData* ResourceManager::createImageData(const std::string& filepath)
    {
        ImageData* pImgData = load_image(filepath);
        uint32_t imgID = ID::generate();
        _images[imgID] = pImgData;
        return pImgData;
    }

    Texture_new* ResourceManager::createTexture(
        TextureSampler sampler,
        uint32_t imageDataID,
        int tiling
    )
    {
        ImageData* pImgData = accessImageData(imageDataID);
        Texture_new* pTexture = nullptr;
        if (pImgData)
        {
            pTexture = Texture_new::create(sampler, pImgData, tiling);
            uint32_t textureID = ID::generate();
            _textures[textureID] = pTexture;
            return pTexture;
        }
        Debug::log(
            "@ResourceManager::createTexture Invalid imageDataID: " + std::to_string(imageDataID),
            Debug::MessageType::PK_ERROR
        );
        return nullptr;
    }

    const ImageData * const ResourceManager::getImageData(uint32_t id) const
    {
        std::unordered_map<uint32_t, ImageData*>::const_iterator it = _images.find(id);
        if (it != _images.end())
            return (*it).second;
        Debug::log(
            "@ResourceManager::getImageData Couldn't find image with id: " + std::to_string(id),
            Debug::MessageType::PK_ERROR
        );
        return nullptr;
    }

    const Texture_new * const ResourceManager::getTexture(uint32_t id) const
    {
        std::unordered_map<uint32_t, Texture_new*>::const_iterator it = _textures.find(id);
        if (it != _textures.end())
            return (*it).second;
        Debug::log(
            "@ResourceManager::getTexture Couldn't find texture with id: " + std::to_string(id),
            Debug::MessageType::PK_ERROR
        );
        return nullptr;
    }

    ImageData* ResourceManager::accessImageData(uint32_t id)
    {
        std::unordered_map<uint32_t, ImageData*>::iterator it = _images.find(id);
        if (it != _images.end())
            return (*it).second;
        Debug::log(
            "@ResourceManager::accessImageData Couldn't find image with id: " + std::to_string(id),
            Debug::MessageType::PK_ERROR
        );
        return nullptr;
    }

    Texture_new* ResourceManager::accessTexture(uint32_t id)
    {
        std::unordered_map<uint32_t, Texture_new*>::iterator it = _textures.find(id);
        if (it != _textures.end())
            return (*it).second;
        Debug::log(
            "@ResourceManager::accessTexture Couldn't find texture with id: " + std::to_string(id),
            Debug::MessageType::PK_ERROR
        );
        return nullptr;
    }
}
