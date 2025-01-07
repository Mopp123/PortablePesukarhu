#pragma once

#include "GUIElement.h"
#include "GUIButton.h"
#include "GUIText.h"
#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/ecs/components/Transform.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/resources/Font.h"
#include "pesukarhu/core/input/InputEvent.h"

#define PK_INPUTFIELD_DEFAULT_HEIGHT 21


namespace pk
{
    namespace ui
    {
        class InputField : public GUIElement
        {
        public:
            class OnSubmitEvent
            {
            public:
                virtual void onSubmit(std::string inputFieldText) = 0;
                virtual ~OnSubmitEvent() {}
            };

        private:
            class InputFieldMouseButtonEvent : public MouseButtonEvent
            {
            private:
                GUIButton* _pButton = nullptr;
                vec3 _originalColor = vec3(0, 0, 0);
                bool _prevSelected = false;

            public:
                InputFieldMouseButtonEvent(GUIButton* pButton);
                virtual ~InputFieldMouseButtonEvent() {}
                virtual void func(InputMouseButtonName button, InputAction action, int mods);
            };

            class InputFieldKeyEvent : public KeyEvent
            {
            private:
                entityID_t _inputFieldEntity = NULL_ENTITY_ID;
                entityID_t _buttonEntity = NULL_ENTITY_ID;
                entityID_t _txtContentEntity = NULL_ENTITY_ID;

                vec3 _buttonOriginalColor;

                OnSubmitEvent* _onSubmit = nullptr;
            public:
                InputFieldKeyEvent(
                    entityID_t inputFieldEntity,
                    entityID_t buttonEntity,
                    entityID_t txtContentEntity,
                    vec3 buttonOriginalColor,
                    OnSubmitEvent* onSubmit
                );
                void func(InputKeyName key, int scancode, InputAction action, int mods);
            };

            class InputFieldCharInputEvent : public CharInputEvent
            {
            private:
                entityID_t _inputFieldEntity = NULL_ENTITY_ID;
                entityID_t _buttonEntity = NULL_ENTITY_ID;
                entityID_t _txtContentEntity = NULL_ENTITY_ID;
                bool _isPassword = false;

            public:
                InputFieldCharInputEvent(
                    entityID_t inputFieldEntity,
                    entityID_t buttonEntity,
                    entityID_t txtContentEntity,
                    bool isPassword
                );
                void func(unsigned int codepoint);
            };

            GUIButton* _pButton = nullptr;
            GUIText* _pInfoText = nullptr;

        public:
            InputField(
                std::string infoTxt,
                const Font& font,
                ConstraintProperties constraintProperties,
                int width,
                OnSubmitEvent* onSubmitEvent,
                bool clearOnSubmit = false,
                vec3 color = { 0.1f, 0.1f, 0.1f },
                vec3 textColor = { 1, 1, 1 },
                vec3 textHighlightColor = { 1, 1, 1 },
                vec3 backgroundHighlightColor = { 0.2f, 0.2f, 0.2f },
                bool password = false
            );
            InputField(const InputField& other) = delete;
            ~InputField();

            void setActive(bool arg);

            // *Button's text element is used as content text
            GUIText* getContentText();

            std::string getContent() const;
            // NOTE: atm theres only way to set "internal content"
            // If using "password" and want to set content requires changing this!
            void setContent(const std::string& str);

            inline GUIButton* getButton() { return _pButton; }
            inline GUIText* getInfoText() { return _pInfoText; }
        };
    }
}
