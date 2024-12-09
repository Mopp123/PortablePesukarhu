#include "UIFactories.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/ecs/components/renderable/GUIRenderable.h"
#include "pesukarhu/ecs/components/renderable/UIRenderableComponent.h"
#include "UIUtils.h"
#include "GUIImage.h"
#include "GUIText.h"
#include "GUIButton.h"
#include <unordered_map>


namespace pk
{
    namespace ui
    {
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
            ) :
                _checkboxEntity(checkboxEntity),
                _checkboxImgEntity(checkboxImgEntity),
                _checkStatusEntity(checkStatusEntity)
            {
            }

            virtual ~CheckboxMouseButtonEvent()
            {
            }

            virtual void func(InputMouseButtonName button, InputAction action, int mods)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                UIElemState* pCheckboxUIState = (UIElemState*)pScene->getComponent(
                    _checkboxEntity,
                    ComponentType::PK_UIELEM_STATE
                );
                UIElemState* pImgUIState = (UIElemState*)pScene->getComponent(
                    _checkboxImgEntity,
                    ComponentType::PK_UIELEM_STATE
                );
                GUIRenderable* pCheckedImg = (GUIRenderable*)pScene->getComponent(
                    _checkStatusEntity,
                    ComponentType::PK_RENDERABLE_GUI
                );

                if (!pCheckboxUIState->isActive() || !pImgUIState->isActive())
                    return;
                if (pImgUIState->mouseOver)
                {
                    if (action == PK_INPUT_PRESS)
                        pImgUIState->state = 1;
                    else if (pImgUIState->state == 1 && action == PK_INPUT_RELEASE)
                        pImgUIState->state = 2;

                    // state: 0 = "nothing"
                    // state: 1 = pressed inside button
                    // state: 2 = pressed and released inside button

                    if (pImgUIState->state == 2)
                    {
                        pCheckboxUIState->checked = !pCheckboxUIState->checked;
                        pCheckedImg->setActive(pCheckboxUIState->checked);
                        pImgUIState->state = 0;
                    }
                }
                else
                {
                    pImgUIState->state = 0;
                }
            }
        };


        class InputFieldKeyEvent : public KeyEvent
        {
        private:
            entityID_t _inputFieldEntity = NULL_ENTITY_ID;
            entityID_t _buttonEntity = NULL_ENTITY_ID;
            entityID_t _txtContentEntity = NULL_ENTITY_ID;

            vec3 _buttonOriginalColor;

            InputFieldOnSubmitEvent* _onSubmit = nullptr;
        public:
            InputFieldKeyEvent(
                entityID_t inputFieldEntity,
                entityID_t buttonEntity,
                entityID_t txtContentEntity,
                vec3 buttonOriginalColor,
                InputFieldOnSubmitEvent* onSubmit
            ) :
                _inputFieldEntity(inputFieldEntity),
                _buttonEntity(buttonEntity),
                _txtContentEntity(txtContentEntity),
                _buttonOriginalColor(buttonOriginalColor),
                _onSubmit(onSubmit)
            {}

            void func(InputKeyName key, int scancode, InputAction action, int mods)
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
            ) :
                _inputFieldEntity(inputFieldEntity),
                _buttonEntity(buttonEntity),
                _txtContentEntity(txtContentEntity),
                _isPassword(isPassword)
            {}

            void func(unsigned int codepoint)
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
        };


        #define PK_INPUTFIELD_DEFAULT_HEIGHT 21
        UIFactoryInputField create_input_field(
            std::string infoTxt, const Font& font,
            ConstraintProperties constraintProperties,
            int width,
            InputFieldOnSubmitEvent* onSubmitEvent,
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

            entityID_t inputFieldEntity = currentScene->createEntity();

            UIElemState* pUIElemState = UIElemState::create(inputFieldEntity);
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
            GUIButton button;
            button.create(
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
            entityID_t buttonRootEntity = button.getEntity();
            entityID_t buttonImgEntity = button.getImage().getEntity();
            entityID_t buttonTxtEntity = button.getText().getEntity();
            // Add blinker to button's text renderable which is concidered the "content string"
            Blinker* pBlinker = Blinker::create(buttonTxtEntity);
            pBlinker->enable = false;

            ConstraintProperties infoTxtConstraintProperties = constraintProperties;
            infoTxtConstraintProperties.horizontalValue += infoDisplacement;
            // Create info txt
            GUIText infoText;
            infoText.create(
                infoTxt, font,
                infoTxtConstraintProperties,
                textColor
            );
            entityID_t infoTextEntity = infoText.getEntity();

            currentScene->addChild(inputFieldEntity, buttonRootEntity);
            currentScene->addChild(inputFieldEntity, infoTextEntity);

            // TESTING
            // NOTE: This should have not worked before... refers to non existent transform...
            Transform* buttonTransform = (Transform*)currentScene->getComponent(
                buttonImgEntity,
                ComponentType::PK_TRANSFORM
            );
            buttonTransform->accessTransformationMatrix()[1 + 3 * 4] -= 4;

            inputManager->addKeyEvent(
                new InputFieldKeyEvent(
                    inputFieldEntity,
                    buttonImgEntity,
                    buttonTxtEntity,
                    color,
                    onSubmitEvent
                )
            );
            inputManager->addCharInputEvent(
                new InputFieldCharInputEvent(
                    inputFieldEntity,
                    buttonImgEntity,
                    buttonTxtEntity,
                    password
                )
            );

            return { inputFieldEntity, buttonTxtEntity };
        }


        float get_text_visual_width(const std::string& txt, const Font* pFont)
        {
            float txtWidth = 0.0f;
            float pos = 0.0f;
            for (char c : txt)
            {
                const FontGlyphData * const glyph = pFont->getGlyph(c);
                if (glyph)
                {
                    txtWidth = pos + glyph->bearingX;
                    pos += ((float)(glyph->advance >> 6));
                }
            }
            return txtWidth;
        }

        UIFactoryCheckbox create_checkbox(
            std::string infoTxt,
            const Font* pFont,
            ConstraintProperties constraintProperties,
            vec3 backgroundColor,
            vec3 backgroundHighlightColor,
            vec3 checkedColor, // The color of the thing indicating checked status
            vec3 textColor
        )
        {
            Scene* pScene = Application::get()->accessCurrentScene();

            entityID_t checkboxEntity = pScene->createEntity();
            UIElemState* pUIElemState = UIElemState::create(checkboxEntity);

            const float defaultWidth = pFont->getPixelSize();
            const float defaultHeight = pFont->getPixelSize();

            float buttonDisplacement = get_text_visual_width(infoTxt, pFont);
            float infoDisplacement = 0;

            // -> No fucking idea what that magic 4 comes from.. but seems good..
            //  ->same as with inputField...
            if (constraintProperties.horizontalType == HorizontalConstraintType::PIXEL_RIGHT)
            {
                buttonDisplacement = 0;
                infoDisplacement = defaultWidth + 4;
            }
            else
            {
                buttonDisplacement += ((float)pFont->getPixelSize()) - 4;
            }

            ConstraintProperties backgroundImgConstraintProperties = constraintProperties;
            backgroundImgConstraintProperties.horizontalValue += buttonDisplacement;
            GUIImage::ImgCreationProperties backgroundImgProperties =
            {
                backgroundImgConstraintProperties,
                defaultWidth,
                defaultHeight,
                backgroundColor,
                backgroundHighlightColor,
                true,
                GUIFilterType::GUI_FILTER_TYPE_ENGRAVE,
                nullptr
            };
            GUIImage backgroundImg;
            backgroundImg.create(backgroundImgProperties);
            entityID_t backgroundImgEntity = backgroundImg.getEntity();

            const float checkedImgMinification = 8.0f; // how many pixels smaller than outer box
            ConstraintProperties checkedImgConstraintProperties = backgroundImgConstraintProperties;
            checkedImgConstraintProperties.horizontalValue = backgroundImgConstraintProperties.horizontalValue + checkedImgMinification * 0.5f;

            float checkedImgDisplacementY = checkedImgMinification * 0.5f;
            if (backgroundImgConstraintProperties.verticalType == VerticalConstraintType::PIXEL_CENTER_VERTICAL)
                checkedImgDisplacementY = -checkedImgMinification * 0.5f;

            checkedImgConstraintProperties.verticalValue = backgroundImgConstraintProperties.verticalValue + checkedImgDisplacementY;
            GUIImage::ImgCreationProperties checkedImgProperties =
            {
                checkedImgConstraintProperties,
                defaultWidth - checkedImgMinification,
                defaultHeight - checkedImgMinification,
                checkedColor,
                checkedColor,
                true,
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS,
                nullptr
            };
            GUIImage checkedImg;
            checkedImg.create(checkedImgProperties);
            entityID_t checkedImgEntity = checkedImg.getEntity();

            // Create info txt
            ConstraintProperties infoTxtConstraintProperties = constraintProperties;
            infoTxtConstraintProperties.horizontalValue += infoDisplacement;
            GUIText infoText;
            infoText.create(
                infoTxt, *pFont, // NOTE: DANGER! TODO: make all funcs here take font as ptr instead of ref!
                infoTxtConstraintProperties,
                textColor
            );
            entityID_t infoTextEntity = infoText.getEntity();

            pScene->addChild(checkboxEntity, backgroundImgEntity);
            pScene->addChild(checkboxEntity, checkedImgEntity);
            pScene->addChild(checkboxEntity, infoTextEntity);

            InputManager* pInputManager = Application::get()->accessInputManager();
            pInputManager->addMouseButtonEvent(
                new CheckboxMouseButtonEvent(
                    checkboxEntity,
                    backgroundImgEntity,
                    checkedImgEntity
                )
            );

            // Set unchecked by default
            set_checkbox_status(checkboxEntity, checkedImgEntity, false);

            return { checkboxEntity, checkedImgEntity };
        }
    }
}
