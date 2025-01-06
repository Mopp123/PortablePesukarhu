#pragma once

#include "pesukarhu/ecs/components/Component.h"
#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/utils/ID.h"
#include <vector>


namespace pk
{
    class TerrainRenderable : public Component
    {
    public:
        // NOTE: TerrainMesh and TerrainMaterial are different types
        // than the regular Mesh and Material types!
        PK_id terrainMeshID = 0;
        PK_id terrainMaterialID = 0;

        TerrainRenderable(
            PK_id terrainMeshID,
            PK_id terrainMaterialID
        );

        // NOTE: TerrainRenderable's mesh and material switched to TerrainMesh and TerrainMaterial
        // from their reqular counterparts recently!
        static TerrainRenderable* create(
            entityID_t target,
            PK_id terrainMeshID,
            PK_id terrainMaterialID
        );
    };
}
