#pragma once

#include "Resource.h"
#include "utils/Image.h"
#include "graphics/Texture.h"
#include "utils/Font.h"
#include "Common.h"
#include "core/Debug.h"
#include <unordered_map>


namespace pk
{
    enum MemoryPoolType
    {
        Images = 0,
        ImageHostPersistentPixelData,
        Textures
    };

    class ResourceManager
    {
    private:
        // Resources which lives through whole execution
        std::unordered_map<uint32_t, Resource*> _persistentResources;
        Texture_new* _pWhiteTexture = nullptr;

        // Resources loaded per scene
        std::unordered_map<uint32_t, Resource*> _resources;

    public:
        ResourceManager();
        ~ResourceManager();
        ResourceManager(const ResourceManager&) = delete;
        void free();

        void createDefaultResources();

        ImageData* loadImage(
            const std::string& filepath
        );

        ImageData* createImage(
            PK_ubyte* pData,
            int width,
            int height,
            int channels
        );

        Texture_new* loadTexture(
            const std::string& filepath,
            TextureSampler sampler
        );

        Texture_new* createTexture(
            uint32_t imageResourceID,
            TextureSampler sampler
        );

        Font* createFont(
            const std::string& filepath,
            int pixelSize
        );

        Resource* getResource(uint32_t id);

        // Just for testing atm!!!
        inline std::unordered_map<uint32_t, Resource*>& getResources() { return _resources; }
        std::vector<Resource*> getResourcesOfType(ResourceType type);

        inline Texture_new* getWhiteTexture() { return _pWhiteTexture; }
    };
}
