#pragma once

#include "ecs/components/Component.h"
#include "utils/ID.h"


namespace pk
{
    class Static3DRenderable : public Component
    {
    public:
        PK_id meshID = 0;

        Static3DRenderable(PK_id meshID) :
            Component(ComponentType::PK_RENDERABLE_STATIC3D),
            meshID(meshID)
        {}
    };
}
