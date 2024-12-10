#include "ResourceManager.h"
#include "pesukarhu/core/Debug.h"
#include "pesukarhu/utils/ID.h"
#include "pesukarhu/utils/ModelLoading.h"
#include "pesukarhu/utils/MeshGenerator.h"

#include <utility>


namespace pk
{
    ResourceManager::ResourceManager()
    {}

    ResourceManager::~ResourceManager()
    {
        freeResources();

        std::unordered_map<uint32_t, Resource*>::iterator itPersistent;
        for (itPersistent = _resources.begin(); itPersistent != _resources.end(); ++itPersistent)
            delete itPersistent->second;
    }

    void ResourceManager::freeResources()
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
        // NOTE: Theres some issue using 3 channels on small textures so using 4 for now..
        PK_ubyte pWhitePixels[2 * 2 * 4];
        memset(pWhitePixels, 255, 2 * 2 * 4);
        ImageData* pWhiteImg = createImage(pWhitePixels, 2, 2, 4, true);
        _pWhiteTexture = createTexture(pWhiteImg->getResourceID(), defaultTextureSampler, true);

        // Black texture
        PK_ubyte pBlackPixels[2 * 2 * 4];
        memset(pBlackPixels, 0, 2 * 2 * 4);
        ImageData* pBlackImg = createImage(pBlackPixels, 2, 2, 4, true);
        _pBlackTexture = createTexture(pBlackImg->getResourceID(), defaultTextureSampler, true);

        _pDefaultMaterial = createMaterial(
            { _pWhiteTexture->getResourceID() },
            0,
            0.0f,
            1.0f,
            { 1, 1, 1, 1 },
            true,
            true
        );
    }

    ImageData* ResourceManager::loadImage(
        const std::string& filepath,
        bool flip,
        bool persistent
    )
    {
        ImageData* pImgData = new ImageData(filepath, flip);
        pImgData->load();
        _resources[pImgData->getResourceID()] = (Resource*)pImgData;
        if (persistent)
            _persistentResources[pImgData->getResourceID()] = (Resource*)pImgData;
        return pImgData;
    }

    ImageData* ResourceManager::createImage(
        PK_ubyte* pData,
        int width,
        int height,
        int channels,
        bool persistent
    )
    {
        ImageData* pImgData = new ImageData(
            pData,
            width,
            height,
            channels
        );
        _resources[pImgData->getResourceID()] = (Resource*)pImgData;
        if (persistent)
            _persistentResources[pImgData->getResourceID()] = (Resource*)pImgData;
        return pImgData;
    }

    Texture* ResourceManager::loadTexture(
        const std::string& filepath,
        TextureSampler sampler,
        bool persistent
    )
    {
        ImageData* pImgData = loadImage(filepath, false, persistent);
        if (!pImgData)
        {
            Debug::log(
                "@ResourceManager::loadTexture "
                "Failed to load image from: " + filepath,
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }

        Texture* pTexture = Texture::create(pImgData->getResourceID(), sampler);
        _resources[pTexture->getResourceID()] = pTexture;
        if (persistent)
            _persistentResources[pTexture->getResourceID()] = pTexture;
        return pTexture;
    }

    Texture* ResourceManager::createTexture(
        uint32_t imageResourceID,
        TextureSampler sampler,
        bool persistent
    )
    {
        if (_resources.find(imageResourceID) == _resources.end())
        {
            Debug::log(
                "@ResourceManager::createTexture "
                "Failed to find image resource with id: " + std::to_string(imageResourceID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        Texture* pTexture = Texture::create(imageResourceID, sampler);
        _resources[pTexture->getResourceID()] = pTexture;
        if (persistent)
            _persistentResources[pTexture->getResourceID()] = pTexture;
        return pTexture;
    }

    Material* ResourceManager::createMaterial(
        const std::vector<uint32_t>& diffuseTextureIDs,
        uint32_t specularTextureID,
        float specularStrength,
        float shininess,
        vec4 color,
        bool shadeless,
        bool persistent
    )
    {
        std::vector<Texture*> textures(diffuseTextureIDs.size());
        for (int i = 0; i < diffuseTextureIDs.size(); ++i)
        {
            Texture* pTexture = (Texture*)getResource(diffuseTextureIDs[i]);
            if (pTexture)
                textures[i] = pTexture;
        }
        // Use black texture as default specular texture if not defined and shininess == 0
        // Use white texture as default specular texture if not defined and shininess > 0
        // *By default all materials require specular texture
        Texture* pSpecularTexture = nullptr;
        if (specularTextureID)
        {
            pSpecularTexture = (Texture*)getResource(specularTextureID);
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
            shininess,
            color,
            shadeless
        );
        _resources[pMaterial->getResourceID()] = pMaterial;
        if (persistent)
            _persistentResources[pMaterial->getResourceID()] = pMaterial;
        return pMaterial;
    }

    TerrainMaterial* ResourceManager::createTerrainMaterial(
        const std::vector<uint32_t>& pChannelTextureIDs,
        uint32_t blendmapTextureID,
        uint32_t customDataTextureID,
        bool persistent
    )
    {
        std::vector<Texture*> textures(pChannelTextureIDs.size());
        for (int i = 0; i < pChannelTextureIDs.size(); ++i)
        {
            Texture* pTexture = (Texture*)getResource(pChannelTextureIDs[i]);
            if (pTexture)
            {
                textures[i] = pTexture;
            }
            else
            {
                Debug::log(
                    "@ResourceManager::createTerrainMaterial "
                    "No textures given for channel: " + std::to_string(i) + " "
                    "Using black as default",
                    Debug::MessageType::PK_WARNING
                );
                textures[i] = _pBlackTexture;
            }
        }

        Texture* pBlendmapTexture = nullptr;
        if (!blendmapTextureID)
        {
            Debug::log(
                "@ResourceManager::createTerrainMaterial "
                "blendmap texture resource ID required!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        pBlendmapTexture = (Texture*)getResource(blendmapTextureID);
        if (!pBlendmapTexture)
        {
            Debug::log(
                "@ResourceManager::createTerrainMaterial "
                "Failed to find blendmap texture with id: " + std::to_string(blendmapTextureID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }

        Texture* pCustomDataTexture = nullptr;
        if (customDataTextureID)
        {
            pCustomDataTexture = (Texture*)getResource(customDataTextureID);
            if (!pCustomDataTexture)
            {
                Debug::log(
                    "@ResourceManager::createTerrainMaterial "
                    "Provided custom data buffer(texture) ID not found: " + std::to_string(customDataTextureID),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
        }
        else
        {
            pCustomDataTexture = getBlackTexture();
        }

        TerrainMaterial* pTerrainMaterial = new TerrainMaterial(
            textures,
            pBlendmapTexture,
            pCustomDataTexture
        );
        _resources[pTerrainMaterial->getResourceID()] = pTerrainMaterial;
        if (persistent)
            _persistentResources[pTerrainMaterial->getResourceID()] = pTerrainMaterial;
        return pTerrainMaterial;
    }

    Mesh* ResourceManager::createMesh(
        const std::vector<Buffer*>& vertexBuffers,
        Buffer* pIndexBuffer,
        uint32_t materialResourceID,
        bool persistent
    )
    {
        Material* pMaterial = (Material*)getResource(materialResourceID);
        Mesh* pMesh = new Mesh(
            vertexBuffers,
            pIndexBuffer,
            pMaterial
        );
        _resources[pMesh->getResourceID()] = pMesh;
        if (persistent)
            _persistentResources[pMesh->getResourceID()] = pMesh;
        return pMesh;
    }

    TerrainMesh* ResourceManager::createTerrainMesh(
        const std::vector<float>& heightmap,
        float tileWidth,
        uint32_t materialResourceID,
        BufferUpdateFrequency updateFrequency
    )
    {
        TerrainMaterial* pTerrainMaterial = (TerrainMaterial*)getResource(materialResourceID);
        std::pair<Buffer*, Buffer*> buffers = generate_terrain_mesh_data(
            heightmap,
            tileWidth,
            updateFrequency
        );

        TerrainMesh* pTerrainMesh = new TerrainMesh(
            buffers.first,
            buffers.second,
            pTerrainMaterial
        );
        _resources[pTerrainMesh->getResourceID()] = pTerrainMesh;
        return pTerrainMesh;
    }

    // TODO: Validate resource ids when adding new resource to _resources!
    Model* ResourceManager::loadModel(
        const std::string& filepath,
        uint32_t materialResourceID,
        bool persistent
    )
    {
        Model* pModel = load_model_gltf(filepath);
        for (Mesh* pMesh : pModel->accessMeshes())
        {
            // atm just testing with single mesh/material!
            Material* pMaterial = (Material*)getResource(materialResourceID);
            pMesh->setMaterial(pMaterial);
            _resources[pMesh->getResourceID()] = pMesh;
            if (persistent)
                _persistentResources[pMesh->getResourceID()] = pMesh;
        }
        _resources[pModel->getResourceID()] = pModel;
        if (persistent)
            _persistentResources[pModel->getResourceID()] = pModel;
        return pModel;
    }

    Model* ResourceManager::createModel(
        const std::vector<uint32_t>& meshResourceIDs,
        bool persistent
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
        if (persistent)
            _persistentResources[pModel->getResourceID()] = pModel;
        return pModel;
    }

    Animation* ResourceManager::createAnimation(
        const Pose& bindPose,
        const std::vector<Pose>& poses
    )
    {
        Animation* pAnimation = new Animation(bindPose, poses);
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

    Resource* ResourceManager::accessResource(uint32_t id)
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

    void ResourceManager::deleteResource(uint32_t id)
    {
        std::unordered_map<uint32_t, Resource*>::iterator it = _resources.find(id);
        if (it == _resources.end())
        {
            Debug::log(
                "@ResourceManager::deleteResource "
                "Failed to find resource with id: " + std::to_string(id),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        delete it->second;
        _resources.erase(id);
        Debug::log(
            "@ResourceManager::deleteResource "
            "Explicitly deleted resource: " + std::to_string(id)
        );

        std::unordered_map<uint32_t, Resource*>::iterator itPersistent = _persistentResources.find(id);
        if (itPersistent != _persistentResources.end())
            _persistentResources.erase(id);
    }

    const Resource* ResourceManager::getResource(uint32_t id) const
    {
        std::unordered_map<uint32_t, Resource*>::const_iterator it = _resources.find(id);
        if (it == _resources.end())
        {
            Debug::log(
                "@ResourceManager::getResource "
                "Failed to find " + Resource::get_resource_type_name(it->second->getType()) + " resource with id: " + std::to_string(id),
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
