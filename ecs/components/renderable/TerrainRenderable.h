#pragma once

#include "ecs/components/Component.h"
#include "utils/ID.h"


namespace pk
{
    class TerrainRenderable : public Component
    {
    public:
        PK_id terrainMaterialID = 0;
        std::vector<float> heightmap;
        float tileWidth = 1.0f;

        TerrainRenderable(uint32_t terrainMaterialID, const std::vector<float>& heightmap, float tileWidth) :
            Component(ComponentType::PK_RENDERABLE_TERRAIN),
            terrainMaterialID(terrainMaterialID),
            heightmap(heightmap),
            tileWidth(tileWidth)
        {}
    };
}
