#pragma once

#include "../Component.h"
#include "../../../graphics/Texture.h"


namespace pk
{
    class Static3DRenderable : public Component
    {
    public:
        Texture* pTexture; // TODO: Material systems things rather than just texture

        Static3DRenderable(Texture* pTexture) :
            Component(ComponentType::PK_RENDERABLE_STATIC3D),
            pTexture(pTexture)
        {}
    };
}
