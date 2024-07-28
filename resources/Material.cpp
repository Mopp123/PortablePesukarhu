#include "Material.h"
#include "core/Debug.h"


namespace pk
{
    Material::Material(
        std::vector<Texture_new*> pDiffuseTextures,
        Texture_new* pSpecularTexture,
        float specularStrength,
        float specularShininess
    ) :
        Resource(ResourceType::RESOURCE_MATERIAL),
        _pSpecularTexture(pSpecularTexture),
        _specularStrength(specularStrength),
        _specularShininess(specularShininess)
    {
        if (pDiffuseTextures.size() > MATERIAL_MAX_DIFFUSE_TEXTURES)
        {
            Debug::log(
                "@Material::Material "
                "Provided too many diffuse textures(" + std::to_string(pDiffuseTextures.size()) + ") "
                "Max material diffuse texture count is " + std::to_string(MATERIAL_MAX_DIFFUSE_TEXTURES),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        memcpy(_pDiffuseTextures, pDiffuseTextures.data(), sizeof(Texture_new*) * pDiffuseTextures.size());
    }

    const Texture_new * const Material::getDiffuseTexture(int index) const
    {
        if (index >= MATERIAL_MAX_DIFFUSE_TEXTURES)
        {
            Debug::log(
                 "@Material::getDiffuseTexture "
                 "index out of bounds! Max material diffuse texture index is: " + std::to_string(MATERIAL_MAX_DIFFUSE_TEXTURES),
                 Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture_new* pTexture = _pDiffuseTextures[index];
        if (!pTexture)
        {
            Debug::log(
                 "@Material::getDiffuseTexture "
                 "No texture assigned to index: " + std::to_string(index),
                 Debug::MessageType::PK_WARNING
            );
        }
        return pTexture;
    }

    Texture_new* Material::accessDiffuseTexture(int index)
    {
        if (index >= MATERIAL_MAX_DIFFUSE_TEXTURES)
        {
            Debug::log(
                 "@Material::accessDiffuseTexture "
                 "index out of bounds! Max material diffuse texture index is: " + std::to_string(MATERIAL_MAX_DIFFUSE_TEXTURES),
                 Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture_new* pTexture = _pDiffuseTextures[index];
        if (!pTexture)
        {
            Debug::log(
                 "@Material::accessDiffuseTexture "
                 "No texture assigned to index: " + std::to_string(index),
                 Debug::MessageType::PK_WARNING
            );
        }
        return pTexture;
    }
}
