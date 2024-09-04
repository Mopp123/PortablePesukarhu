#pragma once

#include "ecs/components/Component.h"
#include "utils/ID.h"


namespace pk
{
    class TerrainRenderable : public Component
    {
    public:
        // NOTE: Don't remember difference between resourceID and PK_id anymore..
        //  -> could be the same..
        PK_id meshID = 0;
        uint32_t terrainMaterialID = 0;

        TerrainRenderable(PK_id meshID, uint32_t terrainMaterialID) :
            Component(ComponentType::PK_RENDERABLE_STATIC3D),
            meshID(meshID),
            terrainMaterialID(terrainMaterialID)
        {}
    };
}
