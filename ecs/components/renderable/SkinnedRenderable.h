#pragma once

#include "ecs/Entity.h"
#include "ecs/components/Component.h"
#include "utils/ID.h"


namespace pk
{
    class SkinnedRenderable : public Component
    {
    public:
        // NOTE:
        //  Currently models loaded with skeletons have the skeleton/bindPose inside the Model
        // TODO:
        //  *Some Skinned mesh thing to be able to have multiple meshes per model as well!
        PK_id modelID = 0;
        PK_id meshID = 0;

        // NOTE:
        // Currently need to know entity which is the "root joint" and also holding the AnimationData component
        entityID_t rootJointEntity = NULL_ENTITY_ID;

        SkinnedRenderable(PK_id modelID, PK_id meshID, entityID_t rootJointEntity) :
            Component(ComponentType::PK_RENDERABLE_SKINNED),
            modelID(modelID),
            meshID(meshID),
            rootJointEntity(rootJointEntity)
        {}
    };
}
