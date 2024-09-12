#include "TerrainMaterial.h"
#include "core/Debug.h"


namespace pk
{
    TerrainMaterial::TerrainMaterial(
        std::vector<Texture_new*> pChannelTextures,
        Texture_new* pBlendmapTexture
    ) :
        Resource(ResourceType::RESOURCE_MATERIAL),
        _pBlendmapTexture(pBlendmapTexture)
    {
        if (pChannelTextures.size() > TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES)
        {
            Debug::log(
                "@TerrainMaterial::TerrainMaterial "
                "Provided too many channel textures(" + std::to_string(pChannelTextures.size()) + ") "
                "Max terrain material channel texture count is " + std::to_string(TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        memcpy(_pChannelTextures, pChannelTextures.data(), sizeof(Texture_new*) * pChannelTextures.size());
    }

    const Texture_new * const TerrainMaterial::getChannelTexture(int channel) const
    {
        if (channel >= TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES)
        {
            Debug::log(
                 "@TerrainMaterial::getChannelTexture "
                 "index out of bounds! Max terrain material texture channel is: " + std::to_string(TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES),
                 Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture_new* pTexture = _pChannelTextures[channel];
        if (!pTexture)
        {
            Debug::log(
                 "@TerrainMaterial::getChannelTexture "
                 "No texture assigned to channel: " + std::to_string(channel),
                 Debug::MessageType::PK_WARNING
            );
        }
        return pTexture;
    }
}
