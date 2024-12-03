#pragma once

#include "Pesukarhu/ecs/Entity.h"
#include "Pesukarhu/ecs/components/Component.h"
#include "Pesukarhu/utils/ID.h"


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

        SkinnedRenderable(PK_id modelID, PK_id meshID);

        static SkinnedRenderable* create(
            entityID_t target,
            PK_id modelID,
            PK_id meshID
        );
    };
}
