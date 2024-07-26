#pragma once

#include "core/Resource.h"
#include "Texture.h"

#define MATERIAL_MAX_DIFFUSE_TEXTURES 3

namespace pk
{
    class Material : public Resource
    {
    private:
        Texture_new* _pDiffuseTextures[MATERIAL_MAX_DIFFUSE_TEXTURES] = { nullptr };
        Texture_new* _pSpecularTexture = nullptr;
        Texture_new* _pNormalTexture = nullptr;

        // NOTE: not used atm cuz currently just testing flat shaded static 3d meshes
        float _specularStrength = 0.0f;
        float _specularShininess = 0.0f;

    public:
        Material(
            std::vector<Texture_new*> pDiffuseTextures,
            Texture_new* pSpecularTexture = nullptr,
            float specularStrength = 1.0f,
            float specularShininess = 32.0f
        );
        Material(const Material&) = delete;

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        const Texture_new * const getDiffuseTexture(int index) const;
        Texture_new* accessDiffuseTexture(int index);
        inline Texture_new* accessSpecularTexture() { return _pSpecularTexture; }
        inline float getSpecularStrength() const { return _specularStrength; }
        inline float getSpecularShininess() const { return _specularShininess; }
    };
}
