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
        Texture* texture = nullptr;
        // TODO: replace "texture" with this after rendering overhaul
        Texture_new* pTexture_new = nullptr;
        vec4 textureCropping;
        vec4 borderColor;
        float borderThickness = 0.0f;

        GUIRenderable(
            Texture* texture = nullptr,
            vec4 textureCropping = vec4(0, 0, 1, 1),
            Texture_new* pTexture_new = nullptr
        ) :
            UIRenderableComponent(ComponentType::PK_RENDERABLE_GUI),
            color(1, 1, 1),
            texture(texture),
            pTexture_new(pTexture_new),
            textureCropping(textureCropping),
            borderColor(vec4(color, 1.0f)),
            borderThickness(0.0f)
        {}

        GUIRenderable(const GUIRenderable& other) :
            UIRenderableComponent(other),
            color(other.color),
            texture(other.texture),
            pTexture_new(other.pTexture_new),
            textureCropping(other.textureCropping),
            borderColor(other.borderColor),
            borderThickness(other.borderThickness)
        {
        }
    };
}
