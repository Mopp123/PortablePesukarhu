#pragma once

#include "GUIElement.h"
#include "GUIImage.h"
#include "GUIText.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/ecs/components/Transform.h"
#include "pesukarhu/ecs/components/renderable/GUIRenderable.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/resources/Font.h"
#include "pesukarhu/core/input/InputEvent.h"


namespace pk
{
    namespace ui
    {
        class GUIButton : public GUIElement
        {
        public:
            // *User defined ui events
            class OnClickEvent
            {
            public:
                virtual void onClick(InputMouseButtonName button) = 0;
                virtual ~OnClickEvent(){}
            };

            struct ButtonCreationProperties
            {
                std::string txt;
                const Font* pFont;
                ConstraintProperties constraintProperties;
                float width;
                float height;
                OnClickEvent* onClick;
                bool selectable = false;
                vec3 selectedColor;
                vec3 color = { 0.1f, 0.1f, 0.1f };
                vec3 textColor = { 1, 1, 1 };
                vec3 textHighlightColor = { 1, 1, 1 };
                vec3 backgroundHighlightColor = { 0.2f, 0.2f, 0.2f };
                GUIFilterType filter = GUIFilterType::GUI_FILTER_TYPE_NONE;
                Texture* pTexture = nullptr;
                vec4 textureCropping = vec4(0, 0, 1, 1);
            };

        private:
            class ButtonMouseButtonEvent : public MouseButtonEvent
            {
            private:
                entityID_t _imgEntity = NULL_ENTITY_ID;
                entityID_t _txtEntity = NULL_ENTITY_ID;

                vec3 _originalColor = vec3(0, 0, 0);
                vec3 _highlightColor = vec3(1.0f, 1.0f, 1.0f);
                OnClickEvent* _onClick = nullptr;
            public:
                ButtonMouseButtonEvent(
                    entityID_t imgEntity,
                    entityID_t txtEntity,
                    vec3 highlightColor,
                    OnClickEvent* onClick
                );
                virtual ~ButtonMouseButtonEvent();
                virtual void func(InputMouseButtonName button, InputAction action, int mods);
            };

            // NOTE: After all sorts of reworks this actually just sets the button's text's highlight atm!
            class ButtonMouseCursorPosEvent : public CursorPosEvent
            {
            private:
                entityID_t _txtEntity = NULL_ENTITY_ID;
                entityID_t _imgEntity = NULL_ENTITY_ID;
                vec3 _txtOriginalColor = vec3(0, 0, 0);
                vec3 _txtHighlightColor = vec3(1, 1, 1);

            public:
                ButtonMouseCursorPosEvent(
                    entityID_t textEntity,
                    entityID_t imgEntity,
                    vec3 txtHighlightColor
                );
                virtual void func(int x, int y);
            };

            GUIImage* _pImage = nullptr;
            GUIText* _pText = nullptr;

        public:
            GUIButton(
                std::string txt,
                const Font& font,
                ConstraintProperties constraintProperties,
                float width, float height,
                OnClickEvent* onClick,
                bool selectable = false,
                vec3 color = { 0.1f, 0.1f, 0.1f },
                vec3 textColor = { 1, 1, 1 },
                vec3 textHighlightColor = { 1, 1, 1 },
                vec3 backgroundHighlightColor = { 0.2f, 0.2f, 0.2f },
                GUIFilterType filter = GUIFilterType::GUI_FILTER_TYPE_NONE,
                Texture* pTexture = nullptr,
                vec4 textureCropping = vec4(0, 0, 1, 1)
            );
            GUIButton(ButtonCreationProperties creationProperties);
            GUIButton(const GUIButton& other) = delete;
            ~GUIButton();

            void setActive(bool arg);

            inline GUIImage* getImage() { return _pImage; }
            inline GUIText* getText() { return _pText; }
        };
    }
}
