#pragma once

#include "Resource.h"
#include "Texture.h"
#include "utils/pkmath.h"

// For each color channel + "blackness channel"
// possible channels = black, red, green, blue and alpha
#define MATERIAL_MAX_DIFFUSE_TEXTURES (5 + 1)

namespace pk
{
    class Material : public Resource
    {
    private:
        Texture* _pDiffuseTextures[MATERIAL_MAX_DIFFUSE_TEXTURES] = { nullptr };
        Texture* _pSpecularTexture = nullptr;
        Texture* _pNormalTexture = nullptr;

        Texture* _pBlendmapTexture = nullptr;

        // NOTE: not used atm cuz currently just testing flat shaded static 3d meshes
        float _specularStrength = 0.0f;
        float _specularShininess = 0.0f;

        vec4 _color = vec4(1, 1, 1, 1);

        bool _shadeless = false;

    public:
        Material(
            std::vector<Texture*> pDiffuseTextures,
            Texture* pSpecularTexture = nullptr,
            float specularStrength = 1.0f,
            float specularShininess = 32.0f,
            Texture* pBlendmapTexture = nullptr,
            vec4 color = vec4(1, 1, 1, 1),
            bool shadeless = false
        );
        Material(const Material&) = delete;

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        const Texture * const getDiffuseTexture(int index) const;
        Texture* accessDiffuseTexture(int index);
        inline const Texture* getSpecularTexture() const { return _pSpecularTexture; }
        inline const Texture* getBlendmapTexture() const { return _pBlendmapTexture; }
        inline float getSpecularStrength() const { return _specularStrength; }
        inline float getSpecularShininess() const { return _specularShininess; }
        inline void setColor(const vec4& color) { _color = color; }
        inline const vec4& getColor() const { return _color; }
        inline void setShadeless(bool arg) { _shadeless = arg; }
        inline bool isShadeless() const { return _shadeless; }
    };
}
