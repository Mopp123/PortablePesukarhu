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
        Texture* texture = nullptr;
        // TODO: replace "texture" with this after rendering overhaul
        Texture_new* pTexture_new = nullptr;
        vec4 textureCropping;
        bool drawBorder = false;

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
            drawBorder(false)
        {}

        GUIRenderable(const GUIRenderable& other) :
            UIRenderableComponent(other),
            color(other.color),
            texture(other.texture),
            pTexture_new(other.pTexture_new),
            textureCropping(other.textureCropping),
            drawBorder(other.drawBorder)
        {
        }
    };
}
