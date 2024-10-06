#pragma once

#include "Resource.h"
#include "Texture.h"

// For each color channel + "blackness channel"
// possible channels = black, red, green, blue and alpha
#define MATERIAL_MAX_DIFFUSE_TEXTURES 5

namespace pk
{
    class Material : public Resource
    {
    private:
        Texture_new* _pDiffuseTextures[MATERIAL_MAX_DIFFUSE_TEXTURES] = { nullptr };
        Texture_new* _pSpecularTexture = nullptr;
        Texture_new* _pNormalTexture = nullptr;

        Texture_new* _pBlendmapTexture = nullptr;

        // NOTE: not used atm cuz currently just testing flat shaded static 3d meshes
        float _specularStrength = 0.0f;
        float _specularShininess = 0.0f;

    public:
        Material(
            std::vector<Texture_new*> pDiffuseTextures,
            Texture_new* pSpecularTexture = nullptr,
            float specularStrength = 1.0f,
            float specularShininess = 32.0f,
            Texture_new* pBlendmapTexture = nullptr
        );
        Material(const Material&) = delete;

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        const Texture_new * const getDiffuseTexture(int index) const;
        Texture_new* accessDiffuseTexture(int index);
        inline const Texture_new* getSpecularTexture() const { return _pSpecularTexture; }
        inline const Texture_new* getBlendmapTexture() const { return _pBlendmapTexture; }
        inline float getSpecularStrength() const { return _specularStrength; }
        inline float getSpecularShininess() const { return _specularShininess; }
    };
}
