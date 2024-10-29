
#pragma once

#include "Resource.h"
#include "Texture.h"
#include "utils/pkmath.h"

// For each color channel + "blackness channel"
// possible channels = black, red, green, blue and alpha + cyan, magenta and yellow
#define TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS (5 + 3)


namespace pk
{
    class TerrainMaterial : public Resource
    {
    private:
        Texture* _pChannelTextures[TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS] = { nullptr };
        Texture* _pBlendmapTexture = nullptr;

    public:
        TerrainMaterial(
            std::vector<Texture*> pChannelTextures,
            Texture* pBlendmapTexture
        );
        TerrainMaterial(const TerrainMaterial&) = delete;

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        const Texture * const getChannelTexture(int index) const;
        Texture* accessChannelTexture(int index);
        inline const Texture* getBlendmapTexture() const { return _pBlendmapTexture; }
    };
}
