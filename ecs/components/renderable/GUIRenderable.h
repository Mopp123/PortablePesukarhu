#pragma once

#include "../Component.h"
#include "../../../utils/pkmath.h"


namespace pk
{
    class GUIRenderable : public Component
    {
    public:
        vec3 color;
        int textureID = 0;
    
        GUIRenderable() :
            Component(ComponentType::PK_RENDERABLE_GUI),
            color(1, 1, 1)
        {}
    };
}
