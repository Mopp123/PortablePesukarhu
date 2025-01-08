#pragma once

#include "GUIElement.h"
#include "GUIImage.h"
#include "GUIText.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/resources/Font.h"
#include "pesukarhu/core/input/InputEvent.h"


namespace pk
{
    namespace ui
    {
        class Checkbox : public GUIElement
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

            const Font* _pFont = nullptr;
            float _checkedImgMinification = 8.0f;

            GUIImage* _pBackground = nullptr;
            GUIImage* _pCheckedStatusIndicator = nullptr;
            GUIText* _pInfoText = nullptr;

        public:
            Checkbox(
                std::string infoTxt,
                const Font* pFont,
                ConstraintProperties constraintProperties,
                vec3 backgroundColor = { 0.2f, 0.2f, 0.2f },
                vec3 backgroundHighlightColor = { 0.4f, 0.4f, 0.4f },
                vec3 checkedColor = { 0.4f, 0.4f, 0.4f }, // The color of the thing indicating checked status
                vec3 textColor = { 1, 1, 1 }
            );
            ~Checkbox();
            Checkbox(const Checkbox& other) = delete;

            virtual void setActive(bool arg);
            virtual void setConstraintValues(float horizontal, float vertical);

            bool isChecked() const;
            void setChecked(bool arg);

            inline GUIImage* getBackgroundImage() { return _pBackground; }
            inline GUIImage* getStatusImage() { return _pCheckedStatusIndicator; }
            inline GUIText* getInfoText() { return _pInfoText; }
        };
    }
}
