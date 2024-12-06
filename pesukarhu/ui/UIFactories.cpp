#include "UIFactories.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/ecs/components/renderable/GUIRenderable.h"
#include "pesukarhu/ecs/components/renderable/UIRenderableComponent.h"
#include "UIUtils.h"
#include "GUIImage.h"
#include <unordered_map>


namespace pk
{
    namespace ui
    {
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
            ) :
                _imgEntity(imgEntity),
                _txtEntity(txtEntity),
                _highlightColor(highlightColor),
                _onClick(onClick)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                GUIRenderable* pImg = (GUIRenderable*)pScene->getComponent(_imgEntity, ComponentType::PK_RENDERABLE_GUI);
                _originalColor = pImg->color;
            }
            // NOTE: ONLY TESTING THIS ATM!!!
            virtual ~ButtonMouseButtonEvent()
            {
                if (_onClick)
                    delete _onClick;
            }
            virtual void func(InputMouseButtonName button, InputAction action, int mods)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                UIElemState* pUIState = (UIElemState*)pScene->getComponent(
                    _imgEntity,
                    ComponentType::PK_UIELEM_STATE
                );
                GUIRenderable* pImg = (GUIRenderable*)pScene->getComponent(
                    _imgEntity,
                    ComponentType::PK_RENDERABLE_GUI
                );

                // If this was used as input field get text blinker...
                // fucking shit way to deal with this stuff atm...
                // TODO: Rework this whole fuckery!
                Blinker* pBlinker = nullptr;
                TextRenderable* pTxtRenderable = nullptr;
                if (_txtEntity)
                {
                    pBlinker = (Blinker*)pScene->getComponent(
                        _txtEntity,
                        ComponentType::PK_BLINKER,
                        false,
                        false
                    );
                    pTxtRenderable = (TextRenderable*)pScene->getComponent(
                        _txtEntity,
                        ComponentType::PK_RENDERABLE_TEXT
                    );
                }

                if (!pUIState->isActive())
                    return;
                if (pUIState->mouseOver)
                {
                    if (action == PK_INPUT_PRESS)
                        pUIState->state = 1;
                    else if (pUIState->state == 1 && action == PK_INPUT_RELEASE)
                        pUIState->state = 2;

                    // state: 0 = "nothing"
                    // state: 1 = pressed inside button
                    // state: 2 = pressed and released inside button

                    if (pUIState->state == 2)
                    {
                        if (pUIState->selectable)
                        {
                            pUIState->selected = !pUIState->selected;
                            pImg->color = _highlightColor;

                            if (pBlinker)
                            {
                                pBlinker->enable = pUIState->selected;
                                // Reset txt to original state if disengage blinker
                                if (!pUIState->selected)
                                    pTxtRenderable->accessVisualStr() = pTxtRenderable->accessStr();
                            }
                        }

                        if (_onClick)
                            _onClick->onClick(button);

                        pUIState->state = 0;
                    }
                }
                else
                {
                    pUIState->selected = false;
                    pImg->color = _originalColor;

                    pUIState->state = 0;

                    if (pBlinker)
                    {
                        pBlinker->enable = false;
                        pTxtRenderable->accessVisualStr() = pTxtRenderable->accessStr();
                    }
                }
            }
        };


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
            ) :
                _txtEntity(textEntity),
                _imgEntity(imgEntity),
                _txtHighlightColor(txtHighlightColor)
            {
                const Scene* pScene = Application::get()->getCurrentScene();
                const TextRenderable* pTxt = (const TextRenderable*)pScene->getComponent(_txtEntity, ComponentType::PK_RENDERABLE_TEXT);
                _txtOriginalColor = pTxt->color;
            }

            virtual void func(int x, int y)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                TextRenderable* pTxt = (TextRenderable*)pScene->getComponent(_txtEntity, ComponentType::PK_RENDERABLE_TEXT);
                UIElemState* pUIState = (UIElemState*)pScene->getComponent(_imgEntity, ComponentType::PK_UIELEM_STATE);
                if (pUIState->mouseOver || pUIState->selected)
                {
                    pTxt->color = _txtHighlightColor;
                }
                else
                {
                    pTxt->color = _txtOriginalColor;
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


        entityID_t create_text(
            const std::string& str, const Font& font,
            ConstraintProperties constraintProperties,
            vec3 color,
            bool bold
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();

            entityID_t entityID = currentScene->createEntity();
            Transform* pTransform = Transform::create(entityID, { 0,0 }, { 1, 1 });

            ConstraintProperties useConstraintProperties = constraintProperties;
            TextRenderable* pRenderable = TextRenderable::create(
                entityID,
                str,
                ((const Resource&)font).getResourceID(),
                color,
                bold
            );
            ConstraintData::create(
                entityID,
                useConstraintProperties
            );


            float width = 0.0f;
            for (char c : str)
            {
                const FontGlyphData * const glyph = font.getGlyph(c);
                if (glyph)
                    width += ((float)(glyph->advance >> 6)) + glyph->bearingX;
            }
            // Need to set correct scale to transform for ui constraint system to work properly!
            pTransform->accessTransformationMatrix()[0 + 0 * 4] = width;
            pTransform->accessTransformationMatrix()[1 + 1 * 4] = font.getPixelSize();

            return entityID;
        }


        UIFactoryButton create_button(
            std::string txt, const Font& font,
            ConstraintProperties constraintProperties,
            float width, float height,
            OnClickEvent* onClick,
            bool selectable,
            vec3 color,
            vec3 textColor,
            vec3 textHighlightColor,
            vec3 backgroundHighlightColor,
            GUIFilterType filter,
            Texture* pTexture,
            vec4 textureCropping
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();
            InputManager* inputManager = Application::get()->accessInputManager();

            entityID_t buttonEntity = currentScene->createEntity();

            GUIImage::ImgCreationProperties imgProperties =
            {
                constraintProperties,
                width,
                height,
                color,
                backgroundHighlightColor,
                true,
                filter,
                pTexture,
                textureCropping
            };
            GUIImage img;
            img.create(imgProperties);
            entityID_t imgEntity = img.getEntity();

            // Add txt displacement
            const int padding = 4;
            const int txtDisplacementX = padding;

            ConstraintProperties useConstraintProperties = constraintProperties;

            if (constraintProperties.horizontalType == HorizontalConstraintType::PIXEL_LEFT ||
                constraintProperties.horizontalType == HorizontalConstraintType::PIXEL_CENTER_HORIZONTAL)
                useConstraintProperties.horizontalValue += (float)txtDisplacementX;
            else if (constraintProperties.horizontalType == HorizontalConstraintType::PIXEL_RIGHT)
                useConstraintProperties.horizontalValue -= (float)txtDisplacementX;

            entityID_t txtEntity = create_text(
                txt, font,
                useConstraintProperties,
                textColor
            );

            currentScene->addChild(buttonEntity, imgEntity);
            currentScene->addChild(buttonEntity, txtEntity);

            Transform* txtTransform = (Transform*)currentScene->getComponent(
                txtEntity,
                ComponentType::PK_TRANSFORM
            );
            // alter transform's scale to make work properly with constraint system
            txtTransform->accessTransformationMatrix()[0 + 0 * 4] = width;
            txtTransform->accessTransformationMatrix()[1 + 1 * 4] = height;

            UIElemState* pUIElemState = (UIElemState*)currentScene->getComponent(
                imgEntity,
                ComponentType::PK_UIELEM_STATE
            );
            pUIElemState->selectable = selectable;

            vec3 selectedColor(0.3f, 0.3f, 0.3f);
            inputManager->addMouseButtonEvent(
                new ButtonMouseButtonEvent(
                    imgEntity,
                    txtEntity,
                    selectedColor,
                    onClick
                )
            );
            inputManager->addCursorPosEvent(
                new ButtonMouseCursorPosEvent(
                    txtEntity,
                    imgEntity,
                    textHighlightColor
                )
            );
            return { buttonEntity, imgEntity, txtEntity };
        }


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
            UIFactoryButton button = create_button(
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
            // Add blinker to button's text renderable which is concidered the "content string"
            Blinker* pBlinker = Blinker::create(button.txtEntity);
            pBlinker->enable = false;

            ConstraintProperties infoTxtConstraintProperties = constraintProperties;
            infoTxtConstraintProperties.horizontalValue += infoDisplacement;
            // Create info txt
            uint32_t infoTxtEntity = create_text(
                infoTxt, font,
                infoTxtConstraintProperties,
                textColor
            );

            currentScene->addChild(inputFieldEntity, button.rootEntity);
            currentScene->addChild(inputFieldEntity, infoTxtEntity);

            // TESTING
            // NOTE: This should have not worked before... refers to non existent transform...
            Transform* buttonTransform = (Transform*)currentScene->getComponent(
                button.imgEntity,
                ComponentType::PK_TRANSFORM
            );
            buttonTransform->accessTransformationMatrix()[1 + 3 * 4] -= 4;

            inputManager->addKeyEvent(
                new InputFieldKeyEvent(
                    inputFieldEntity,
                    button.imgEntity,
                    button.txtEntity,
                    color,
                    onSubmitEvent
                )
            );
            inputManager->addCharInputEvent(
                new InputFieldCharInputEvent(
                    inputFieldEntity,
                    button.imgEntity,
                    button.txtEntity,
                    password
                )
            );

            return { inputFieldEntity, button.txtEntity };
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
            uint32_t infoTxtEntity = create_text(
                infoTxt, *pFont, // NOTE: DANGER! TODO: make all funcs here take font as ptr instead of ref!
                infoTxtConstraintProperties,
                textColor
            );

            pScene->addChild(checkboxEntity, backgroundImgEntity);
            pScene->addChild(checkboxEntity, checkedImgEntity);
            pScene->addChild(checkboxEntity, infoTxtEntity);

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
