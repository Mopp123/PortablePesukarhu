#pragma once

#include "Resource.h"
#include "Texture.h"

// Can have textures for channels
// "black", "red", "green", "blue", "alpha"
#define TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES 5

namespace pk
{
    class TerrainMaterial : public Resource
    {
    private:
        Texture_new* _pChannelTextures[TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES] = { nullptr };
        Texture_new* _pBlendmapTexture = nullptr;

    public:
        TerrainMaterial(
            std::vector<Texture_new*> pChannelTextures,
            Texture_new* pBlendmapTexture = nullptr
        );
        TerrainMaterial(const TerrainMaterial&) = delete;

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        const Texture_new * const getChannelTexture(int channel) const;
        inline const Texture_new* getBlendmapTexture() const { return _pBlendmapTexture; }
    };
}
