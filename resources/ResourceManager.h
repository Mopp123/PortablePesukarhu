#pragma once

#include "Resource.h"
#include "Image.h"
#include "Texture.h"
#include "Material.h"
#include "TerrainMaterial.h"
#include "Mesh.h"
#include "TerrainMesh.h"
#include "Model.h"
#include "Animation.h"
#include "Font.h"
#include "Common.h"
#include "core/Debug.h"
#include <vector>
#include <unordered_map>


namespace pk
{
    enum MemoryPoolType
    {
        Images = 0,
        ImageHostPersistentPixelData,
        Textures
    };

    class ResourceManager
    {
    private:
        // Resources which lives through whole execution
        std::unordered_map<uint32_t, Resource*> _persistentResources;
        Texture* _pWhiteTexture = nullptr;
        Texture* _pBlackTexture = nullptr;

        Material* _pDefaultMaterial = nullptr;

        // Resources loaded per scene
        std::unordered_map<uint32_t, Resource*> _resources;

    public:
        ResourceManager();
        ~ResourceManager();
        ResourceManager(const ResourceManager&) = delete;
        void freeResources();

        void createDefaultResources();

        ImageData* loadImage(
            const std::string& filepath,
            bool flip = false,
            bool persistent = false
        );

        ImageData* createImage(
            PK_ubyte* pData,
            int width,
            int height,
            int channels,
            bool persistent = false
        );

        Texture* loadTexture(
            const std::string& filepath,
            TextureSampler sampler,
            bool persistent = false
        );

        Texture* createTexture(
            uint32_t imageResourceID,
            TextureSampler sampler,
            bool persistent = false
        );

        Material* createMaterial(
            const std::vector<uint32_t>& diffuseTextureIDs,
            uint32_t specularTextureID = 0,
            float specularStrength = 0.0f,
            float shininess = 1.0f,
            uint32_t blendmapTextureID = 0,
            vec4 color = vec4(1, 1, 1, 1),
            bool shadeless = false,
            bool persistent = false
        );

        TerrainMaterial* createTerrainMaterial(
            const std::vector<uint32_t>& pChannelTextureIDs,
            uint32_t blendmapTextureID,
            bool persistent = false
        );

        Mesh* createMesh(
            const std::vector<Buffer*>& vertexBuffers,
            Buffer* pIndexBuffer,
            uint32_t materialResourceID,
            bool persistent = false
        );

        // NOTE: "Material" here changed to "TerrainMaterial" type recently!
        TerrainMesh* createTerrainMesh(
            const std::vector<float>& heightmap,
            float tileWidth,
            uint32_t terrainMaterialResourceID,
            BufferUpdateFrequency updateFrequency = BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STATIC
        );

        Model* loadModel(
            const std::string& filepath,
            uint32_t materialResourceID,
            bool persistent = false
        );

        Model* createModel(
            const std::vector<uint32_t>& meshResourceIDs,
            bool persistent = false
        );

        Animation* createAnimation(
            const Pose& bindPose,
            const std::vector<Pose>& poses
        );

        Font* createFont(
            const std::string& filepath,
            int pixelSize
        );

        void deleteResource(uint32_t id);

        // TODO: Resource type checking!
        Resource* accessResource(uint32_t id);
        // TODO: Resource type checking!
        const Resource* getResource(uint32_t id) const;

        // Just for testing atm!!!
        inline std::unordered_map<uint32_t, Resource*>& getResources() { return _resources; }
        std::vector<Resource*> getResourcesOfType(ResourceType type);

        inline Texture* getWhiteTexture() { return _pWhiteTexture; }
        inline const Texture* getWhiteTexture() const { return _pWhiteTexture; }

        inline Texture* getBlackTexture() { return _pBlackTexture; }
        inline const Texture* getBlackTexture() const { return _pBlackTexture; }

        inline const Material* getDefaultMaterial() const { return _pDefaultMaterial; }
    };
}
