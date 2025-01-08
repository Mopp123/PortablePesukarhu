
#pragma once

#include "Resource.h"
#include "Texture.h"
#include "pesukarhu/utils/pkmath.h"
#include <vector>

// Possible color channels for blending:
// *black
// *red
// *green
// *blue
// *alpha
#define TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS 5


namespace pk
{
    class TerrainMaterial : public Resource
    {
    private:
        Texture* _pChannelTextures[TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS] = { nullptr };
        Texture* _pBlendmapTexture = nullptr;
        // Quite confusing name I know... atm used to be able to provide some additional
        // info in addition to blendmap only
        // NOTE: Atm experimenting this being just grayscale texture!
        Texture* _pCustomDataTexture = nullptr;
        int _textureTiling = 1;

    public:
        TerrainMaterial(
            std::vector<Texture*> pChannelTextures,
            Texture* pBlendmapTexture,
            Texture* pCustomDataTexture = nullptr,
            int textureTiling = 1
        );
        TerrainMaterial(const TerrainMaterial&) = delete;

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        const Texture * const getChannelTexture(int index) const;
        Texture* accessChannelTexture(int index);
        void setChannelTexture(unsigned int index, Texture* pTexture);
        inline const Texture* getBlendmapTexture() const { return _pBlendmapTexture; }
        inline const Texture* getCustomDataTexture() const { return _pCustomDataTexture; }

        inline int getTextureTiling() const { return _textureTiling; }
    };
}
