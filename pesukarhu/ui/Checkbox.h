#pragma once

#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/resources/Font.h"
#include "pesukarhu/core/input/InputEvent.h"
#include "GUIImage.h"
#include "GUIText.h"


namespace pk
{
    namespace ui
    {
        class Checkbox
        {
        private:
            class CheckboxMouseButtonEvent : public MouseButtonEvent
            {
            private:
                entityID_t _checkboxEntity = NULL_ENTITY_ID;
                entityID_t _checkboxImgEntity = NULL_ENTITY_ID;
                entityID_t _checkStatusEntity = NULL_ENTITY_ID;

            public:
                CheckboxMouseButtonEvent(
                    entityID_t checkboxEntity,
                    entityID_t checkboxImgEntity,
                    entityID_t checkStatusEntity
                );
                virtual ~CheckboxMouseButtonEvent() {}
                virtual void func(InputMouseButtonName button, InputAction action, int mods);
            };

            entityID_t _entity = NULL_ENTITY_ID;
            GUIImage _background;
            GUIImage _checkedStatusIndicator;
            GUIText _infoText;

        public:
            Checkbox() {}
            Checkbox(const Checkbox& other);
            void create(
                std::string infoTxt,
                const Font* pFont,
                ConstraintProperties constraintProperties,
                vec3 backgroundColor = { 0.2f, 0.2f, 0.2f },
                vec3 backgroundHighlightColor = { 0.4f, 0.4f, 0.4f },
                vec3 checkedColor = { 0.4f, 0.4f, 0.4f }, // The color of the thing indicating checked status
                vec3 textColor = { 1, 1, 1 }
            );

            void setActive(bool arg);
            bool isChecked() const;
            void setChecked(bool arg);

            inline entityID_t getEntity() const { return _entity; }
        };
    }
}