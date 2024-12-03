#pragma once

#include "Pesukarhu/ecs/components/Component.h"
#include "Pesukarhu/ecs/Entity.h"
#include "Pesukarhu/utils/ID.h"


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
