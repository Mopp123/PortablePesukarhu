#pragma once

#include "UIRenderableComponent.h"
#include "resources/Texture.h"
#include "utils/pkmath.h"


namespace pk
{
    class GUIRenderable : public UIRenderableComponent
    {
    public:
        vec3 color;
        // TODO: replace "texture" with this after rendering overhaul
        Texture* pTexture = nullptr;
        vec4 textureCropping;
        vec4 borderColor;
        float borderThickness = 0.0f;

        GUIRenderable(
            vec4 textureCropping = vec4(0, 0, 1, 1),
            Texture* pTexture = nullptr
        ) :
            UIRenderableComponent(ComponentType::PK_RENDERABLE_GUI),
            color(1, 1, 1),
            pTexture(pTexture),
            textureCropping(textureCropping),
            borderColor(vec4(color, 1.0f)),
            borderThickness(0.0f)
        {}

        GUIRenderable(const GUIRenderable& other) :
            UIRenderableComponent(other),
            color(other.color),
            pTexture(other.pTexture),
            textureCropping(other.textureCropping),
            borderColor(other.borderColor),
            borderThickness(other.borderThickness)
        {
        }
    };
}
