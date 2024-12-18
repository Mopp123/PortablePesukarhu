#pragma once

#include "GUIElement.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/ecs/components/renderable/GUIRenderable.h"
#include "pesukarhu/ecs/components/Transform.h"


namespace pk
{
    namespace ui
    {
        class GUIImage : public GUIElement
        {
        public:
            // TODO: Put this outside of GUIImage class
            struct ImgCreationProperties
            {
                ConstraintProperties constraintProperties;
                float width;
                float height;
                vec3 color = { 1, 1, 1 };
                vec3 highlightColor = { 1, 1, 1 };
                bool useHighlightColor = false;
                GUIFilterType filter = GUIFilterType::GUI_FILTER_TYPE_NONE;
                Texture* pTexture = nullptr;
                vec4 textureCropping = vec4(0, 0, 1, 1);
            };

        public:
            GUIImage(ImgCreationProperties creationProperties);
            GUIImage(const GUIImage& other) = delete;
            ~GUIImage();

            GUIRenderable* getRenderable();
            ConstraintData* getConstraint();
            Transform* getTransform();

            void setActive(bool arg);
        };
    }
}
