#include "ResourceManager.h"
#include "core/Debug.h"
#include "utils/ID.h"
#include "utils/ModelLoading.h"


namespace pk
{
    ResourceManager::ResourceManager()
    {}

    ResourceManager::~ResourceManager()
    {
        free();

        std::unordered_map<uint32_t, Resource*>::iterator itPersistent;
        for (itPersistent = _resources.begin(); itPersistent != _resources.end(); ++itPersistent)
            delete itPersistent->second;
    }

    void ResourceManager::free()
    {
        std::unordered_map<uint32_t, Resource*>::iterator it;
        for (it = _resources.begin(); it != _resources.end(); ++it)
        {
            if (_persistentResources.find(it->first) == _persistentResources.end())
                delete it->second;
        }
        _resources.clear();
        // Re add all persistent ones... quite fukin dumb, but will do for now..
        std::unordered_map<uint32_t, Resource*>::iterator itPersistent;
        for (itPersistent = _persistentResources.begin(); itPersistent != _persistentResources.end(); ++itPersistent)
            _resources[itPersistent->second->getResourceID()] = itPersistent->second;
        Debug::log("ResourceManager freed all resources!");
    }

    void ResourceManager::createDefaultResources()
    {
        // Texture to be used all stuff which uses color instead of texture.
        //
        // Those require some texture which we just multiply by color.
        // This is done because want to use same shaders for all similar stuff.
        // (Current rendering systems gets fucked up if some gl attrib or uniform location
        // is not used and returns -1)


        TextureSampler defaultTextureSampler;

        // White texture
        // NOTE: Don't remember why I put 4 channels here instead of 1...
        // ..there might not even be reason for that....
        PK_ubyte pWhitePixels[2 * 2 * 4];
        memset(pWhitePixels, 255, 2 * 2 * 4);
        ImageData* pWhiteImg = createImage(pWhitePixels, 2, 2, 4);
        _pWhiteTexture = createTexture(pWhiteImg->getResourceID(), defaultTextureSampler);
        _persistentResources[pWhiteImg->getResourceID()] = pWhiteImg;
        _persistentResources[_pWhiteTexture->getResourceID()] = _pWhiteTexture;

        // Black texture
        PK_ubyte pBlackPixels[2 * 2 * 3];
        memset(pBlackPixels, 0, 2 * 2 * 3);
        ImageData* pBlackImg = createImage(pBlackPixels, 2, 2, 3);
        _pBlackTexture = createTexture(pBlackImg->getResourceID(), defaultTextureSampler);
        _persistentResources[pBlackImg->getResourceID()] = pBlackImg;
        _persistentResources[_pBlackTexture->getResourceID()] = _pBlackTexture;
    }

    ImageData* ResourceManager::loadImage(
        const std::string& filepath
    )
    {
        ImageData* pImgData = new ImageData(filepath);
        pImgData->load();
        _resources[pImgData->getResourceID()] = (Resource*)pImgData;
        return pImgData;
    }

    ImageData* ResourceManager::createImage(
        PK_ubyte* pData,
        int width,
        int height,
        int channels
    )
    {
        ImageData* pImgData = new ImageData(
            pData,
            width,
            height,
            channels
        );
        _resources[pImgData->getResourceID()] = (Resource*)pImgData;
        return pImgData;
    }

    Texture_new* ResourceManager::loadTexture(
        const std::string& filepath,
        TextureSampler sampler
    )
    {
        Debug::notify_unimplemented("ResourceManager::loadTexture#1");
        return nullptr;
    }

    Texture_new* ResourceManager::createTexture(
        uint32_t imageResourceID,
        TextureSampler sampler
    )
    {
        if (_resources.find(imageResourceID) == _resources.end())
        {
            Debug::log(
                "@ResourceManager::loadTexture "
                "Failed to find image resource with id: " + std::to_string(imageResourceID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture_new* pTexture = Texture_new::create(imageResourceID, sampler);
        _resources[pTexture->getResourceID()] = pTexture;
        return pTexture;
    }

    Material* ResourceManager::createMaterial(
        const std::vector<uint32_t>& diffuseTextureIDs,
        uint32_t specularTextureID,
        float specularStrength,
        float shininess
    )
    {
        std::vector<Texture_new*> textures(diffuseTextureIDs.size());
        for (int i = 0; i < diffuseTextureIDs.size(); ++i)
        {
            Texture_new* pTexture = (Texture_new*)getResource(diffuseTextureIDs[i]);
            if (pTexture)
                textures[i] = pTexture;
        }
        // Use black texture as default specular texture if not defined and shininess == 0
        // Use white texture as default specular texture if not defined and shininess > 0
        // *By default all materials require specular texture
        Texture_new* pSpecularTexture = nullptr;
        if (specularTextureID)
        {
            pSpecularTexture = (Texture_new*)getResource(specularTextureID);
            if (!pSpecularTexture)
            {
                Debug::log(
                    "@ResourceManager::createMaterial "
                    "Failed to find specular texture with id: " + std::to_string(specularTextureID),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                pSpecularTexture = _pBlackTexture;
            }
        }
        else
        {
            if (shininess <= 0.0f)
                pSpecularTexture = _pBlackTexture;
            else
                pSpecularTexture = _pWhiteTexture;
        }
        Material* pMaterial = new Material(
            textures,
            pSpecularTexture,
            specularStrength,
            shininess
        );
        _resources[pMaterial->getResourceID()] = pMaterial;
        return pMaterial;
    }

    Mesh* ResourceManager::createMesh(
        Buffer* pVertexBuffer,
        Buffer* pIndexBuffer,
        const VertexBufferLayout& layout,
        uint32_t materialResourceID
    )
    {
        Material* pMaterial = (Material*)getResource(materialResourceID);
        Mesh* pMesh = new Mesh(
            pVertexBuffer,
            pIndexBuffer,
            pMaterial,
            layout
        );
        _resources[pMesh->getResourceID()] = pMesh;
        return pMesh;
    }

    // TODO: Validate resource ids when adding new resource to _resources!
    Model* ResourceManager::loadModel(
        const std::string& filepath,
        uint32_t materialResourceID
    )
    {
        Model* pModel = load_model_glb(filepath);
        for (Mesh* pMesh : pModel->accessMeshes())
        {
            // atm just testing with single mesh/material!
            Material* pMaterial = (Material*)getResource(materialResourceID);
            pMesh->setMaterial(pMaterial);
            _resources[pMesh->getResourceID()] = pMesh;
        }

        _resources[pModel->getResourceID()] = pModel;
        return pModel;
    }

    Model* ResourceManager::createModel(
        const std::vector<uint32_t>& meshResourceIDs
    )
    {
        std::vector<Mesh*> pMeshes(meshResourceIDs.size());
        for (int i = 0; i < meshResourceIDs.size(); ++i)
        {
            Mesh* pMesh = (Mesh*)getResource(meshResourceIDs[i]);
            if (pMesh)
                pMeshes[i] = pMesh;
        }
        Model* pModel = new Model(pMeshes);
        _resources[pModel->getResourceID()] = pModel;
        return pModel;
    }

    Animation* ResourceManager::createAnimation(
        const std::vector<Pose>& poses
    )
    {
        Animation* pAnimation = new Animation(poses);
        _resources[pAnimation->getResourceID()] = pAnimation;
        return pAnimation;
    }

    Font* ResourceManager::createFont(
        const std::string& filepath,
        int pixelSize
    )
    {
        Font* pFont = new Font(filepath, pixelSize);
        pFont->load();
        // Ycm fucks up here for some fucking weird reason..
        // gives non existing error? compiles just fine tho..
        _resources[pFont->getResourceID()] = pFont;
        return pFont;
    }

    Resource* ResourceManager::getResource(uint32_t id)
    {
        std::unordered_map<uint32_t, Resource*>::iterator it = _resources.find(id);
        if (it == _resources.end())
        {
            Debug::log(
                "@ResourceManager::getResource "
                "Failed to find resource with id: " + std::to_string(id),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        return it->second;
    }

    const Resource* ResourceManager::getResource(uint32_t id) const
    {
        std::unordered_map<uint32_t, Resource*>::const_iterator it = _resources.find(id);
        if (it == _resources.end())
        {
            Debug::log(
                "@ResourceManager::getResource(2) "
                "Failed to find resource with id: " + std::to_string(id),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        return it->second;
    }

    std::vector<Resource*> ResourceManager::getResourcesOfType(ResourceType type)
    {
        std::vector<Resource*> foundResources;
        std::unordered_map<uint32_t, Resource*>::iterator it;
        for (it = _resources.begin(); it != _resources.end(); ++it)
        {
            if (it->second->getType() == type)
                foundResources.push_back(it->second);
        }
        return foundResources;
    }
}
