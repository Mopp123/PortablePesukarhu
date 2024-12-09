#pragma once

#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/ecs/components/renderable/GUIRenderable.h"
#include "pesukarhu/ecs/components/Transform.h"


namespace pk
{
    namespace ui
    {
        class GUIImage
        {
        private:
            entityID_t _entity = NULL_ENTITY_ID;

        public:
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
            GUIImage() {}
            GUIImage(const GUIImage& other);
            void create(ImgCreationProperties creationProperties);

            GUIRenderable* getRenderable();
            Transform* getTransform();

            void setActive(bool arg);

            inline entityID_t getEntity() const { return _entity; }
        };
    }
}
