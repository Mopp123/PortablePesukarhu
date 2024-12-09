#include "InputField.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    namespace ui
    {
        InputField::InputFieldKeyEvent::InputFieldKeyEvent(
            entityID_t inputFieldEntity,
            entityID_t buttonEntity,
            entityID_t txtContentEntity,
            vec3 buttonOriginalColor,
            OnSubmitEvent* onSubmit
        ) :
            _inputFieldEntity(inputFieldEntity),
            _buttonEntity(buttonEntity),
            _txtContentEntity(txtContentEntity),
            _buttonOriginalColor(buttonOriginalColor),
            _onSubmit(onSubmit)
        {}

        void InputField::InputFieldKeyEvent::func(
            InputKeyName key,
            int scancode,
            InputAction action,
            int mods
        )
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            UIElemState* pInputFieldUIState = (UIElemState*)pScene->getComponent(
                _inputFieldEntity,
                ComponentType::PK_UIELEM_STATE
            );
            UIElemState* pButtonUIState = (UIElemState*)pScene->getComponent(
                _buttonEntity,
                ComponentType::PK_UIELEM_STATE
            );
            TextRenderable* pTextRenderable = (TextRenderable*)pScene->getComponent(
                _txtContentEntity,
                ComponentType::PK_RENDERABLE_TEXT
            );
            Blinker* pBlinker = (Blinker*)pScene->getComponent(
                _txtContentEntity,
                ComponentType::PK_BLINKER
            );

            if (!(pButtonUIState->isActive() && pInputFieldUIState->isActive()))
                return;
            // NOTE: Not sure can this be used?
            bool inputModeActive = pButtonUIState->selected;
            if (inputModeActive)
            {
                // <#M_DANGER> Not sure how badly this 'll end up fucking something...
                std::string& contentTxt = pInputFieldUIState->content;
                std::string& renderableTxt = pTextRenderable->accessStr();

                if (action != PK_INPUT_RELEASE)
                {
                    // Erasing front text if backspace
                    if (key == PK_INPUT_KEY_BACKSPACE)
                    {
                        if (!contentTxt.empty())
                            contentTxt.pop_back();
                        if (!renderableTxt.empty())
                            renderableTxt.pop_back();
                    }

                    if (key == PK_INPUT_KEY_ENTER)
                    {
                        if (_onSubmit)
                            _onSubmit->onSubmit(pInputFieldUIState->content);

                        if (pInputFieldUIState->clearOnSubmit)
                        {
                            contentTxt.clear();
                            renderableTxt.clear();
                        }

                        pButtonUIState->selected = false;
                        pBlinker->enable = false;
                        pTextRenderable->accessVisualStr() = renderableTxt;

                        // test resetting to original color
                        GUIRenderable* pImg = (GUIRenderable*)pScene->getComponent(
                            _buttonEntity,
                            ComponentType::PK_RENDERABLE_GUI
                        );
                        pImg->color = _buttonOriginalColor;
                        // NOTE: previously changed border color here.
                        //  -> atm could be used to switch from emboss to engrave?
                        //pImg->borderColor = _buttonOriginalBorderColor;
                    }
                }
            }
        }


        InputField::InputFieldCharInputEvent::InputFieldCharInputEvent(
            entityID_t inputFieldEntity,
            entityID_t buttonEntity,
            entityID_t txtContentEntity,
            bool isPassword
        ) :
            _inputFieldEntity(inputFieldEntity),
            _buttonEntity(buttonEntity),
            _txtContentEntity(txtContentEntity),
            _isPassword(isPassword)
        {}

        void InputField::InputFieldCharInputEvent::func(unsigned int codepoint)
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            UIElemState* pInputFieldState = (UIElemState*)pScene->getComponent(
                _inputFieldEntity,
                ComponentType::PK_UIELEM_STATE
            );
            UIElemState* pButtonUIState = (UIElemState*)pScene->getComponent(
                _buttonEntity,
                ComponentType::PK_UIELEM_STATE
            );
            TextRenderable* pContentText = (TextRenderable*)pScene->getComponent(
                _txtContentEntity,
                ComponentType::PK_RENDERABLE_TEXT
            );

            if (!pButtonUIState->isActive())
                return;
            bool inputModeActive = pButtonUIState->selected;
            if (inputModeActive)
            {
                // <#M_DANGER>
                // just for testing atm.. quite dumb way to do it like this..
                unsigned char typedChar = (unsigned char)codepoint;

                // Always add the "real content" to UIElemState but
                // if password don't put to TextRenderable
                pInputFieldState->content.push_back(typedChar);
                std::string& txt = pContentText->accessStr();
                if (!_isPassword)
                    txt.push_back(typedChar);
                else
                    txt.push_back('*');
            }
        }


        InputField::InputField(const InputField& other) :
            _entity(other._entity),
            _button(other._button),
            _infoText(other._infoText)
        {
        }

        void InputField::create(
            std::string infoTxt, const Font& font,
            ConstraintProperties constraintProperties,
            int width,
            OnSubmitEvent* onSubmitEvent,
            bool clearOnSubmit,
            vec3 color,
            vec3 textColor,
            vec3 textHighlightColor,
            vec3 backgroundHighlightColor,
            bool password
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();
            InputManager* inputManager = Application::get()->accessInputManager();

            _entity = currentScene->createEntity();

            UIElemState* pUIElemState = UIElemState::create(_entity);
            pUIElemState->selectable = true;
            pUIElemState->clearOnSubmit = clearOnSubmit;

            // figure out text's size
            float infoTxtWidth = 0.0f;//font.getPixelSize();
            float pos = 0.0f;
            for (char c : infoTxt)
            {
                const FontGlyphData * const glyph = font.getGlyph(c);
                if (glyph)
                {
                    infoTxtWidth = pos + glyph->bearingX;
                    pos += ((float)(glyph->advance >> 6));
                }
            }
            // TODO: Comment wtf happening here!!!!!!!
            // NOTE: Still something fucked about this!!
            //  -> No fucking idea what that magic 4 comes from.. but seems good..
            float buttonDisplacementX = infoTxtWidth;
            float buttonDisplacementY = 0.0f;
            float infoDisplacement = 0;
            if (constraintProperties.horizontalType == HorizontalConstraintType::PIXEL_RIGHT)
            {
                buttonDisplacementX = 0;
                infoDisplacement = width + 4;
            }
            else
            {
                buttonDisplacementX += ((float)font.getPixelSize()) - 4;
            }

            ConstraintProperties buttonConstraintProperties = constraintProperties;
            buttonConstraintProperties.horizontalValue += buttonDisplacementX;
            buttonConstraintProperties.verticalValue += buttonDisplacementY;

            // Create button
            _button.create(
                "", // txt
                font,
                buttonConstraintProperties,
                (float)width, font.getPixelSize(), // scale
                nullptr, // onclick
                true, // selectable
                color,
                textColor,
                textHighlightColor,
                backgroundHighlightColor,
                GUIFilterType::GUI_FILTER_TYPE_ENGRAVE, // filter type
                nullptr, // texture
                {0, 0, 1, 1} // cropping
            );
            entityID_t buttonRootEntity = _button.getEntity();
            entityID_t buttonImgEntity = _button.getImage().getEntity();
            _contentText = _button.getText();
            entityID_t contentTextEntity = _contentText.getEntity();
            // Add blinker to button's text renderable which is concidered the "content string"
            Blinker* pBlinker = Blinker::create(contentTextEntity);
            pBlinker->enable = false;

            ConstraintProperties infoTxtConstraintProperties = constraintProperties;
            infoTxtConstraintProperties.horizontalValue += infoDisplacement;
            // Create info txt
            _infoText.create(
                infoTxt, font,
                infoTxtConstraintProperties,
                textColor
            );
            entityID_t infoTextEntity = _infoText.getEntity();

            currentScene->addChild(_entity, buttonRootEntity);
            currentScene->addChild(_entity, infoTextEntity);

            // TESTING
            // NOTE: This should have not worked before... refers to non existent transform...
            Transform* buttonTransform = (Transform*)currentScene->getComponent(
                buttonImgEntity,
                ComponentType::PK_TRANSFORM
            );
            buttonTransform->accessTransformationMatrix()[1 + 3 * 4] -= 4;

            inputManager->addKeyEvent(
                new InputFieldKeyEvent(
                    _entity,
                    buttonImgEntity,
                    contentTextEntity,
                    color,
                    onSubmitEvent
                )
            );
            inputManager->addCharInputEvent(
                new InputFieldCharInputEvent(
                    _entity,
                    buttonImgEntity,
                    contentTextEntity,
                    password
                )
            );
        }

        void InputField::setActive(bool arg)
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@InputField::setActive "
                    "InputField's entity was null. "
                    "Make sure InputField was created successfully using InputField::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            _button.setActive(arg);
            _infoText.setActive(arg);
            Scene* pScene = Application::get()->accessCurrentScene();
            for (Component* pComponent : pScene->getComponents(_entity))
                pComponent->setActive(arg);
        }

        std::string InputField::getContent() const
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            const UIElemState* pState = (const UIElemState*)pScene->getComponent(
                _entity,
                ComponentType::PK_UIELEM_STATE
            );
            if (!pState)
            {
                Debug::log(
                    "@InputField::getContent "
                    "No valid UIElemState found for entity: " + std::to_string(_entity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return "";
            }
            return pState->content;
        }

        void InputField::setContent(const std::string& str)
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            UIElemState* pUIElemState = (UIElemState*)pScene->getComponent(
                _entity,
                ComponentType::PK_UIELEM_STATE
            );
            TextRenderable* pContentTextRenderable = _contentText.getRenderable();
            if (!pUIElemState)
            {
                Debug::log(
                    "@InputField::setContent "
                    "No valid UIElemState found for input field root entity: " + std::to_string(_entity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            if (!pContentTextRenderable)
            {
                Debug::log(
                    "@InputField::setContent "
                    "No valid TextRenderable found for input field's content entity: " + std::to_string(_contentText.getEntity()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            pUIElemState->content = str;
            pContentTextRenderable->accessStr() = str;
        }
    }
}