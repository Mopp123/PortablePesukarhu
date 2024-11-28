#pragma once

#include "ecs/components/Component.h"
#include "utils/ID.h"


namespace pk
{
    class TerrainRenderable : public Component
    {
    public:
        // NOTE: TerrainMesh and TerrainMaterial are different types
        // than the regular Mesh and Material types!
        PK_id terrainMeshID = 0;
        PK_id terrainMaterialID = 0;
        std::vector<float> heightmap;
        float tileWidth = 1.0f;

        TerrainRenderable(PK_id terrainMeshID, PK_id terrainMaterialID, const std::vector<float>& heightmap, float tileWidth) :
            Component(ComponentType::PK_RENDERABLE_TERRAIN),
            terrainMeshID(terrainMeshID),
            terrainMaterialID(terrainMaterialID),
            heightmap(heightmap),
            tileWidth(tileWidth)
        {}
    };
}
