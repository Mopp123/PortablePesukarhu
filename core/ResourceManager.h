#pragma once

#include "utils/Image.h"
#include "utils/Font.h"
#include "graphics/Texture.h"
#include <unordered_map>


namespace pk
{
    class ResourceManager
    {
    private:
        std::unordered_map<uint32_t, ImageData*> _images;
        std::unordered_map<uint32_t, Texture_new*> _textures;

    public:
        ResourceManager();
        ~ResourceManager();

        ImageData* createImageData(const std::string& filepath);

        Texture_new* createTexture(
            TextureSampler sampler,
            uint32_t imageDataID,
            int tiling = 1
        );

        const ImageData * const getImageData(uint32_t id) const;
        const Texture_new * const getTexture(uint32_t id) const;

        ImageData* accessImageData(uint32_t id);
        Texture_new* accessTexture(uint32_t id);
    };
}
