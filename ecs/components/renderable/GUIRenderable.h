#pragma once

#include "UIRenderableComponent.h"
#include "../../../graphics/Texture.h"
#include "../../../utils/pkmath.h"


namespace pk
{
    class GUIRenderable : public UIRenderableComponent
    {
    public:
        vec3 color;
        Texture* texture;
        vec4 textureCropping;
        bool drawBorder = false;

        GUIRenderable(Texture* texture = nullptr, vec4 textureCropping = vec4(0, 0, 1, 1)) :
            UIRenderableComponent(ComponentType::PK_RENDERABLE_GUI),
            color(1, 1, 1),
            texture(texture),
            textureCropping(textureCropping),
            drawBorder(false)
        {}
    };
}
