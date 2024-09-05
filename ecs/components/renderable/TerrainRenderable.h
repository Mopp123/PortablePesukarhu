#pragma once

#include "ecs/components/Component.h"
#include "utils/ID.h"


namespace pk
{
    class TerrainRenderable : public Component
    {
    public:
        PK_id meshID = 0;
        PK_id materialID = 0;
        std::vector<float> heightmap;
        float tileWidth = 1.0f;

        TerrainRenderable(PK_id meshID, PK_id materialID, const std::vector<float>& heightmap, float tileWidth) :
            Component(ComponentType::PK_RENDERABLE_TERRAIN),
            meshID(meshID),
            materialID(materialID),
            heightmap(heightmap),
            tileWidth(tileWidth)
        {}
    };
}
