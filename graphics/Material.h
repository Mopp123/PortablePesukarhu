#pragma once

#include "core/Resource.h"
#include "Texture.h"

#define MATERIAL_MAX_TEXTURES 3

namespace pk
{
    class Material : public Resource
    {
    private:
        Texture_new* _pTextures[MATERIAL_MAX_TEXTURES] = { nullptr };

        // NOTE: not used atm cuz currently just testing flat shaded static 3d meshes
        float _specularStrength = 0.0f;
        float _specularShininess = 0.0f;

    public:
        Material(std::vector<Texture_new*> pTextures);
        Material(const Material&) = delete;

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        const Texture_new * const getTexture(int index) const;
        Texture_new* accessTexture(int index);
    };
}
