#include "UIFactories.h"
#include "../../../core/Application.h"
#include "../../components/renderable/GUIRenderable.h"
#include "../../components/renderable/UIRenderableComponent.h"
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

        // TODO: Make cursor pos event for ALL GUI IMAGES
        // (and maybe txt as well) to track if cursor is on ui
        class ImageMouseCursorPosEvent : public CursorPosEvent
        {
        private:
            entityID_t _entity = NULL_ENTITY_ID;
            vec3 _originalColor = vec3(0, 0, 0);
            vec3 _highlightColor = vec3(1, 1, 1);

        public:
            ImageMouseCursorPosEvent(
                entityID_t entity,
                vec3 highlightColor
            ) :
                _entity(entity),
                _highlightColor(highlightColor)
            {
                const Scene* pScene = Application::get()->getCurrentScene();
                const GUIRenderable* pImg = (const GUIRenderable*)pScene->getComponent(_entity, ComponentType::PK_RENDERABLE_GUI);
                _originalColor = pImg->color;
            }

            virtual void func(int x, int y)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                GUIRenderable* pImg = (GUIRenderable*)pScene->getComponent(_entity, ComponentType::PK_RENDERABLE_GUI);
                const Transform* pTransform = (const Transform*)pScene->getComponent(_entity, ComponentType::PK_TRANSFORM);
                UIElemState* pUIState = (UIElemState*)pScene->getComponent(_entity, ComponentType::PK_UIELEM_STATE);

                const mat4& tMat = pTransform->getTransformationMatrix();

                float width = tMat[0 + 0 * 4];
                float height = tMat[1 + 1 * 4];

                float xPos = tMat[0 + 3 * 4];
                float yPos = tMat[1 + 3 * 4];

                const int imgLayer = pImg->getLayer();
                // *ignore highlight coloring if selected
                // set or remove highlight coloring
                if (pUIState->isActive() && x >= xPos && x <= xPos + width && y <= yPos && y >= yPos - height)
                {
                    // Make sure theres no overlap with other ui (layer vals check)
                    UIElemState::s_pickedLayers[_entity] = imgLayer;

                    // find highest layer value and make it selected
                    int highest = 0;
                    std::unordered_map<entityID_t, int>::const_iterator it;
                    for (it = UIElemState::s_pickedLayers.begin(); it != UIElemState::s_pickedLayers.end(); ++it)
                    {
                        highest = std::max(highest, it->second);
                    }
                    UIRenderableComponent::set_current_selected_layer(highest);

                    if (UIRenderableComponent::get_current_selected_layer() > pImg->getLayer())
                    {
                        // Reset mouseover to false if "layer switch"
                        if (!pUIState->selected)
                            pImg->color = _originalColor;

                        pUIState->mouseOver = false;
                        return;
                    }
                    UIRenderableComponent::set_current_selected_layer(pImg->getLayer());

                    if (!pUIState->selected)
                        pImg->color = _highlightColor;

                    pUIState->mouseOver = true;
                }
                else
                {
                    if (UIElemState::s_pickedLayers.find(_entity) != UIElemState::s_pickedLayers.end())
                        UIElemState::s_pickedLayers.erase(_entity);

                    if (!pUIState->selected)
                        pImg->color = _originalColor;

                    pUIState->mouseOver = false;
                }
            }
        };

        // TODO: Make cursor pos event for ALL GUI IMAGES
        // (and maybe txt as well) to track if cursor is on ui
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
            vec4 _buttonOriginalBorderColor;

            InputFieldOnSubmitEvent* _onSubmit = nullptr;
        public:
            InputFieldKeyEvent(
                entityID_t inputFieldEntity,
                entityID_t buttonEntity,
                entityID_t txtContentEntity,
                vec3 buttonOriginalColor,
                vec4 buttonOriginalBorderColor,
                InputFieldOnSubmitEvent* onSubmit
            ) :
                _inputFieldEntity(inputFieldEntity),
                _buttonEntity(buttonEntity),
                _txtContentEntity(txtContentEntity),
                _buttonOriginalColor(buttonOriginalColor),
                _buttonOriginalBorderColor(buttonOriginalBorderColor),
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
                            pImg->borderColor = _buttonOriginalBorderColor;
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


        entityID_t create_image(
            HorizontalConstraintType horizontalType, float horizontalVal,
            VerticalConstraintType verticalType, float verticalVal,
            float width, float height,
            vec3 color,
            vec4 borderColor,
            float borderThickness,
            Texture* pTexture,
            vec4 textureCropping
        )
        {
	        Application* app = Application::get();
	        Scene* currentScene = app->accessCurrentScene();

	        entityID_t entityID = currentScene->createEntity();
            Transform::create(entityID, { 0,0 }, { width, height });
            currentScene->createGUIRenderable(
                entityID,
                pTexture,
                color,
                borderColor,
                borderThickness,
                textureCropping
            );
            currentScene->createUIConstraint(
                entityID,
                horizontalType,
                horizontalVal,
                verticalType,
                verticalVal
            );

            UIElemState* pUIElemState = currentScene->createUIElemState(entityID);
            pUIElemState->selectable = false;

            InputManager* pInputManager = Application::get()->accessInputManager();
            pInputManager->addCursorPosEvent(
                new ImageMouseCursorPosEvent(
                    entityID,
                    color // Highlight color
                )
            );

            return entityID;
        }


        entityID_t create_image(ImgCreationProperties creationProperties)
        {
	        Application* app = Application::get();
	        Scene* currentScene = app->accessCurrentScene();

	        entityID_t entityID = currentScene->createEntity();
            Transform::create(
                entityID,
                { 0,0 },
                { creationProperties.width, creationProperties.height }
            );
            currentScene->createGUIRenderable(
                entityID,
                creationProperties.pTexture,
                creationProperties.color,
                creationProperties.borderColor,
                creationProperties.borderThickness,
                creationProperties.textureCropping
            );
            currentScene->createUIConstraint(
                entityID,
                creationProperties.constraintProperties.horizontalType,
                creationProperties.constraintProperties.horizontalValue,
                creationProperties.constraintProperties.verticalType,
                creationProperties.constraintProperties.verticalValue
            );

            UIElemState* pUIElemState = currentScene->createUIElemState(entityID);
            pUIElemState->selectable = false;

            vec3 highlightColor = creationProperties.useHighlightColor ? creationProperties.highlightColor : creationProperties.color;
            InputManager* pInputManager = Application::get()->accessInputManager();
            pInputManager->addCursorPosEvent(
                new ImageMouseCursorPosEvent(
                    entityID,
                    highlightColor
                )
            );

            return entityID;
        }


        std::pair<entityID_t, TextRenderable*> create_text(
            const std::string& str, const Font& font,
            HorizontalConstraintType horizontalType, float horizontalVal,
            VerticalConstraintType verticalType, float verticalVal,
            vec3 color,
            bool bold
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();

            entityID_t entityID = currentScene->createEntity();
            Transform* pTransform = Transform::create(entityID, { 0,0 }, { 1, 1 });

            TextRenderable* pRenderable = currentScene->createTextRenderable(
                entityID,
                str,
                ((const Resource&)font).getResourceID(),
                color,
                bold
            );
            currentScene->createUIConstraint(
                entityID,
                horizontalType,
                horizontalVal,
                verticalType,
                verticalVal
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

            return std::make_pair(entityID, pRenderable);
        }


        UIFactoryButton create_button(
            std::string txt, const Font& font,
            HorizontalConstraintType horizontalType, float horizontalVal,
            VerticalConstraintType verticalType, float verticalVal,
            float width, float height,
            OnClickEvent* onClick,
            bool selectable,
            vec3 color,
            vec3 textColor,
            vec3 textHighlightColor,
            vec3 backgroundHighlightColor,
            vec4 borderColor,
            float borderThickness,
            Texture* pTexture,
            vec4 textureCropping
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();
            InputManager* inputManager = Application::get()->accessInputManager();

            entityID_t buttonEntity = currentScene->createEntity();

            ImgCreationProperties imgProperties =
            {
                {
                    horizontalType, horizontalVal,
                    verticalType, verticalVal
                },
                width,
                height,
                color,
                backgroundHighlightColor,
                true,
                borderColor,
                borderThickness,
                pTexture,
                textureCropping
            };
            entityID_t imgEntity = create_image(imgProperties);

            // Add txt displacement
            const int padding = 4;
            const int txtDisplacementX = padding;

            if (horizontalType == HorizontalConstraintType::PIXEL_LEFT ||
                horizontalType == HorizontalConstraintType::PIXEL_CENTER_HORIZONTAL)
                horizontalVal += (float)txtDisplacementX;
            else if (horizontalType == HorizontalConstraintType::PIXEL_RIGHT)
                horizontalVal -= (float)txtDisplacementX;

            entityID_t txtEntity = create_text(
                txt, font,
                horizontalType, horizontalVal,
                verticalType, verticalVal,
                textColor
            ).first;

            currentScene->addChild(buttonEntity, imgEntity);
            currentScene->addChild(buttonEntity, txtEntity);

            GUIRenderable* imgRenderable = (GUIRenderable*)currentScene->getComponent(
                imgEntity,
                ComponentType::PK_RENDERABLE_GUI
            );
            Transform* imgTransform = (Transform*)currentScene->getComponent(
                imgEntity,
                ComponentType::PK_TRANSFORM
            );
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
            HorizontalConstraintType horizontalType, float horizontalVal,
            VerticalConstraintType verticalType, float verticalVal,
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

            UIElemState* pUIElemState = currentScene->createUIElemState(inputFieldEntity);
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
            float buttonDisplacement = infoTxtWidth;
            float infoDisplacement = 0;
            if (horizontalType == HorizontalConstraintType::PIXEL_RIGHT)
            {
                buttonDisplacement = 0;
                infoDisplacement = width + 4;
            }
            else
            {
                buttonDisplacement += ((float)font.getPixelSize()) - 4;
            }

            // Create button
            vec4 borderColor(color.x, color.y, color.z, 1.0f);
            UIFactoryButton button = create_button(
                "", // txt
                font,
                horizontalType, horizontalVal + buttonDisplacement, // horiz. constraint
                verticalType, verticalVal, // vert. constraint
                (float)width, font.getPixelSize(), // scale
                nullptr, // onclick
                true, // selectable
                color,
                textColor,
                textHighlightColor,
                backgroundHighlightColor,
                borderColor,
                2, // border thickness
                nullptr, // texture
                {0, 0, 1, 1} // cropping
            );
            // Add blinker to button's text renderable which is concidered the "content string"
            Blinker* pBlinker = currentScene->createBlinker(button.txtEntity);
            pBlinker->enable = false;

            // Create info txt
            uint32_t infoTxtEntity = create_text(
                infoTxt, font,
                horizontalType, horizontalVal + infoDisplacement, // *Add displacement to info text, so its to the right of the box
                verticalType, verticalVal,
                textColor
            ).first;

            currentScene->addChild(inputFieldEntity, button.rootEntity);
            currentScene->addChild(inputFieldEntity, infoTxtEntity);

            // NOTE: button's text component is used as the InputField's "content text"
            TextRenderable* contentText = (TextRenderable*)currentScene->getComponent(
                button.txtEntity,
                ComponentType::PK_RENDERABLE_TEXT
            );

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
                    borderColor,
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
    }
}
