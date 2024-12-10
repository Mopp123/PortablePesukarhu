#pragma once

#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/ecs/components/Transform.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/resources/Font.h"
#include "pesukarhu/core/input/InputEvent.h"
#include "GUIButton.h"
#include "GUIText.h"

#define PK_INPUTFIELD_DEFAULT_HEIGHT 21


namespace pk
{
    namespace ui
    {
        class InputField
        {
        public:
            class OnSubmitEvent
            {
            public:
                virtual void onSubmit(std::string inputFieldText) = 0;
                virtual ~OnSubmitEvent() {}
            };

        private:
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

            entityID_t _entity = NULL_ENTITY_ID;
            GUIButton _button;
            GUIText _contentText;
            GUIText _infoText;

        public:
            InputField() {}
            InputField(const InputField& other);
            void create(
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

            void setActive(bool arg);

            std::string getContent() const;
            // NOTE: atm theres only way to set "internal content"
            // If using "password" and want to set content requires changing this!
            void setContent(const std::string& str);

            inline entityID_t getEntity() const { return _entity; }
            inline GUIButton& getButton() { return _button; }
            inline GUIText& getText() { return _infoText; }
        };
    }
}
