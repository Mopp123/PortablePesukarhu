#pragma once

#include "UIRenderableComponent.h"
#include "Pesukarhu/ecs/Entity.h"
#include "Pesukarhu/resources/Texture.h"
#include "Pesukarhu/utils/pkmath.h"


namespace pk
{
    enum GUIFilterType
    {
        GUI_FILTER_TYPE_NONE = 0,
        GUI_FILTER_TYPE_EMBOSS = 1,
        GUI_FILTER_TYPE_ENGRAVE = 2
    };

    class GUIRenderable : public UIRenderableComponent
    {
    public:
        vec3 color;
        // TODO: replace "texture" with this after rendering overhaul
        Texture* pTexture = nullptr;

        // NOTE: Texture cropping is a bit funky atm.
        // z and w are width and height of a single "cropped tile" in uv space (0,0 - 1,1 value range)
        // x and y is texture offset in this "tile space"
        // This is because final uv coord in shader is calculated as:
        //  uvCoord = (uvCoord + croppingPos) * croppingScale;
        vec4 textureCropping;
        GUIFilterType filter = GUIFilterType::GUI_FILTER_TYPE_NONE;

        GUIRenderable(vec4 textureCropping, Texture* pTexture);
        GUIRenderable(const GUIRenderable& other);

        static GUIRenderable* create(
            entityID_t target,
            Texture* pTexture,
            vec3 color,
            GUIFilterType filter,
            vec4 textureCropping
        );
    };
}
