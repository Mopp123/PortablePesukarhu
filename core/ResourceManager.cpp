#include "ResourceManager.h"
#include "core/Debug.h"
#include "utils/ID.h"


namespace pk
{
    ResourceManager::ResourceManager()
    {}

    ResourceManager::~ResourceManager()
    {
        free();
    }

    void ResourceManager::free()
    {
        std::unordered_map<uint32_t, Resource*>::iterator it;
        for (it = _resources.begin(); it != _resources.end(); ++it)
            delete it->second;
        _resources.clear();
        Debug::log("ResourceManager freed all resources!");
    }

    ImageData* ResourceManager::loadImage(
        const std::string& filepath
    )
    {
        ImageData* pImgData = new ImageData(filepath);
        pImgData->load();
        _resources[pImgData->getResourceID()] = (Resource*)pImgData;
        return pImgData;
    }

    ImageData* ResourceManager::createImage(
        PK_ubyte* pData,
        int width,
        int height,
        int channels
    )
    {
        ImageData* pImgData = new ImageData(
            pData,
            width,
            height,
            channels
        );
        _resources[pImgData->getResourceID()] = (Resource*)pImgData;
        return pImgData;
    }

    Texture_new* ResourceManager::loadTexture(
        const std::string& filepath,
        TextureSampler sampler
    )
    {
        Debug::notify_unimplemented("ResourceManager::loadTexture#1");
    }

    Texture_new* ResourceManager::createTexture(
        uint32_t imageResourceID,
        TextureSampler sampler
    )
    {
        if (_resources.find(imageResourceID) == _resources.end())
        {
            Debug::log(
                "@ResourceManager::loadTexture "
                "Failed to find image resource with id: " + std::to_string(imageResourceID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture_new* pTexture = Texture_new::create(imageResourceID, sampler);
        _resources[pTexture->getResourceID()] = pTexture;
        return pTexture;
    }

    Font* ResourceManager::createFont(
        const std::string& filepath,
        int pixelSize
    )
    {
        Font* pFont = new Font(filepath, pixelSize);
        pFont->load();
        // Ycm fucks up here for some fucking weird reason..
        // gives non existing error? compiles just fine tho..
        _resources[pFont->getResourceID()] = pFont;
        return pFont;
    }

    Resource* ResourceManager::getResource(uint32_t id)
    {
        std::unordered_map<uint32_t, Resource*>::iterator it = _resources.find(id);
        if (it == _resources.end())
        {
            Debug::log(
                "@ResourceManager::getResource "
                "Failed to find resource with id: " + std::to_string(id),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        return it->second;
    }

    std::vector<Resource*> ResourceManager::getResourcesOfType(ResourceType type)
    {
        std::vector<Resource*> foundResources;
        std::unordered_map<uint32_t, Resource*>::iterator it;
        for (it = _resources.begin(); it != _resources.end(); ++it)
        {
            if (it->second->getType() == type)
                foundResources.push_back(it->second);
        }
        return foundResources;
    }
}
