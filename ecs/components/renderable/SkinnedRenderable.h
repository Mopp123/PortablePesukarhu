#pragma once

#include "ecs/Entity.h"
#include "ecs/components/Component.h"
#include "utils/ID.h"


namespace pk
{
    class SkinnedRenderable : public Component
    {
    public:
        PK_id meshID = 0;
        entityID_t skeletonEntity = NULL_ENTITY_ID;

        SkinnedRenderable(PK_id meshID, entityID_t skeletonEntity) :
            Component(ComponentType::PK_RENDERABLE_SKINNED),
            meshID(meshID),
            skeletonEntity(skeletonEntity)
        {}
    };
}
