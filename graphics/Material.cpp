#include "Material.h"
#include "core/Debug.h"


namespace pk
{
    Material::Material(std::vector<Texture_new*> pTextures) :
        Resource(ResourceType::RESOURCE_MATERIAL)
    {
        if (pTextures.size() > MATERIAL_MAX_TEXTURES)
        {
            Debug::log(
                "@Material::Material "
                "Provided too many textures(" + std::to_string(pTextures.size()) + ") "
                "Max material texture count is " + std::to_string(MATERIAL_MAX_TEXTURES),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        memcpy(_pTextures, pTextures.data(), sizeof(Texture_new*) * pTextures.size());
    }

    const Texture_new * const Material::getTexture(int index) const
    {
        if (index >= MATERIAL_MAX_TEXTURES)
        {
            Debug::log(
                 "@Material::getTexture "
                 "index out of bounds! Max material texture index is: " + std::to_string(MATERIAL_MAX_TEXTURES),
                 Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture_new* pTexture = _pTextures[index];
        if (!pTexture)
        {
            Debug::log(
                 "@Material::getTexture "
                 "No texture assigned to index: " + std::to_string(index),
                 Debug::MessageType::PK_WARNING
            );
        }
        return pTexture;
    }

    Texture_new* Material::accessTexture(int index)
    {
        if (index >= MATERIAL_MAX_TEXTURES)
        {
            Debug::log(
                 "@Material::accessTexture "
                 "index out of bounds! Max material texture index is: " + std::to_string(MATERIAL_MAX_TEXTURES),
                 Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture_new* pTexture = _pTextures[index];
        if (!pTexture)
        {
            Debug::log(
                 "@Material::accessTexture "
                 "No texture assigned to index: " + std::to_string(index),
                 Debug::MessageType::PK_WARNING
            );
        }
        return pTexture;
    }
}
