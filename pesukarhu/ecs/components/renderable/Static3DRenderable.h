#pragma once

#include "pesukarhu/ecs/components/Component.h"
#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/utils/ID.h"


namespace pk
{
    class Static3DRenderable : public Component
    {
    public:
        PK_id meshID = 0;

        Static3DRenderable(PK_id meshID);

        static Static3DRenderable* create(entityID_t target, PK_id meshID);
    };
}
