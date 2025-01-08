#include "TerrainMaterial.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
    TerrainMaterial::TerrainMaterial(
        std::vector<Texture*> pChannelTextures,
        Texture* pBlendmapTexture,
        Texture* pCustomDataTexture,
        int textureTiling
    ) :
        Resource(ResourceType::RESOURCE_TERRAIN_MATERIAL),
        _pBlendmapTexture(pBlendmapTexture),
        _pCustomDataTexture(pCustomDataTexture),
        _textureTiling(textureTiling)
    {
        if (pChannelTextures.size() > TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS)
        {
            Debug::log(
                "@TerrainMaterial::TerrainMaterial "
                "Provided too many channel textures(" + std::to_string(pChannelTextures.size()) + ") "
                "Max terrain material channel texture count is " + std::to_string(TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        if (!_pBlendmapTexture)
        {
            Debug::log(
                "@TerrainMaterial::TerrainMaterial "
                "pBlendmapTexture required!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        memcpy(_pChannelTextures, pChannelTextures.data(), sizeof(Texture*) * pChannelTextures.size());
    }

    const Texture * const TerrainMaterial::getChannelTexture(int index) const
    {
        if (index >= TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS)
        {
            Debug::log(
                 "@TerrainMaterial::getChannelTexture "
                 "index: " + std::to_string(index) + " "
                 "out of bounds! Max terrain material channel texture index is: " + std::to_string(TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS),
                 Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture* pTexture = _pChannelTextures[index];
        if (!pTexture)
        {
            Debug::log(
                 "@TerrainMaterial::getChannelTexture "
                 "No texture assigned to index: " + std::to_string(index),
                 Debug::MessageType::PK_WARNING
            );
        }
        return pTexture;
    }

    Texture* TerrainMaterial::accessChannelTexture(int index)
    {
        if (index >= TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS)
        {
            Debug::log(
                 "@TerrainMaterial::accessChannelTexture "
                 "index out of bounds! Max terrain material channel texture index is: " + std::to_string(TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS),
                 Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture* pTexture = _pChannelTextures[index];
        if (!pTexture)
        {
            Debug::log(
                 "@TerrainMaterial::accessChannelTexture "
                 "No texture assigned to index: " + std::to_string(index),
                 Debug::MessageType::PK_WARNING
            );
        }
        return pTexture;
    }

    void TerrainMaterial::setChannelTexture(unsigned int index, Texture* pTexture)
    {
        if (index >= TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS)
        {
            Debug::log(
                 "@TerrainMaterial::setChannelTexture "
                 "index: " + std::to_string(index) + " "
                 "out of bounds! Max terrain material channel texture index is: " + std::to_string(TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS),
                 Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        _pChannelTextures[index] = pTexture;
    }
}
