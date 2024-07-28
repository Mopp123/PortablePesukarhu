#include "UIFactories.h"
#include "../../../core/Application.h"
#include "../../components/renderable/GUIRenderable.h"
#include "../../components/renderable/UIRenderableComponent.h"


namespace pk
{
    namespace ui
    {
        class ButtonMouseButtonEvent : public MouseButtonEvent
        {
        private:
            entityID_t _buttonEntity = NULL_ENTITY_ID;
            entityID_t _imgEntity = NULL_ENTITY_ID;

            vec3 _originalColor = vec3(0, 0, 0);
            vec3 _highlightColor = vec3(1.0f, 1.0f, 1.0f);
            OnClickEvent* _onClick = nullptr;
        public:
            ButtonMouseButtonEvent(
                entityID_t buttonEntity,
                entityID_t imgEntity,
                vec3 highlightColor,
                OnClickEvent* onClick
            ) :
                _buttonEntity(buttonEntity),
                _imgEntity(imgEntity),
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
                UIElemState* pUIState = (UIElemState*)pScene->getComponent(_buttonEntity, ComponentType::PK_UIELEM_STATE);
                GUIRenderable* pImg = (GUIRenderable*)pScene->getComponent(_imgEntity, ComponentType::PK_RENDERABLE_GUI);

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
                }
            }
        };

        // TODO: Make cursor pos event for ALL GUI IMAGES
        // (and maybe txt as well) to track if cursor is on ui
        class ButtonMouseCursorPosEvent : public CursorPosEvent
        {
        private:
            entityID_t _buttonEntity = NULL_ENTITY_ID;
            entityID_t _imgEntity = NULL_ENTITY_ID;
            vec3 _originalColor = vec3(0, 0, 0);
            vec3 _highlightColor = vec3(1, 1, 1);

        public:
            ButtonMouseCursorPosEvent(
                entityID_t buttonEntity,
                entityID_t imgEntity,
                vec3 highlightColor
            ) :
                _buttonEntity(buttonEntity),
                _imgEntity(imgEntity),
                _highlightColor(highlightColor)
            {
                const Scene* pScene = Application::get()->getCurrentScene();
                const GUIRenderable* pImg = (const GUIRenderable*)pScene->getComponent(_imgEntity, ComponentType::PK_RENDERABLE_GUI);
                _originalColor = pImg->color;
            }

            virtual void func(int x, int y)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                GUIRenderable* pImg = (GUIRenderable*)pScene->getComponent(_imgEntity, ComponentType::PK_RENDERABLE_GUI);
                const Transform* pTransform = (const Transform*)pScene->getComponent(_imgEntity, ComponentType::PK_TRANSFORM);
                UIElemState* pUIState = (UIElemState*)pScene->getComponent(_buttonEntity, ComponentType::PK_UIELEM_STATE);

                const mat4& tMat = pTransform->getTransformationMatrix();

                float width = tMat[0 + 0 * 4];
                float height = tMat[1 + 1 * 4];

                float xPos = tMat[0 + 3 * 4];
                float yPos = tMat[1 + 3 * 4];
                // *ignore highlight coloring if selected
                // set or remove highlight coloring
                if (pUIState->isActive() && x >= xPos && x <= xPos + width && y <= yPos && y >= yPos - height)
                {
                    // TODO: that layer stuff which is fucked atm...
                    // Make sure theres no overlap with other ui (layer vals check)
                    /*
                    int imgLayer = _pImg->getLayerVal();
                    int currentLayer = UIRenderableComponent::get_current_selected_layer();

                    if (UIRenderableComponent::get_current_selected_layer() > _pImg->getLayerVal())
                    {
                        // Reset mouseover to false if "layer switch"
                        if (!_stateRef.selected)
                            _pImg->color = _originalColor;

                        _stateRef.mouseOver = false;
                        return;
                    }
                    UIRenderableComponent::set_current_selected_layer(_pImg->getLayerVal());
                    */

                    if (!pUIState->selected)
                        pImg->color = _highlightColor;
                    pUIState->mouseOver = true;
                }
                else
                {
                    // Reset current selected layer if needed to..
                    /*
                    if (UIRenderableComponent::get_current_selected_layer() == _pImg->getLayerVal())
                        UIRenderableComponent::set_current_selected_layer(0);
                    */

                    if (!pUIState->selected)
                        pImg->color = _originalColor;
                    pUIState->mouseOver = false;
                }
            }
        };


        class InputFieldKeyEvent : public KeyEvent
        {
        private:
            entityID_t _buttonEntity = NULL_ENTITY_ID;
            entityID_t _inputFieldEntity = NULL_ENTITY_ID;

            InputFieldOnSubmitEvent* _onSubmit = nullptr;
        public:
            InputFieldKeyEvent(
                entityID_t buttonEntity,
                entityID_t inputFieldEntity,
                InputFieldOnSubmitEvent* onSubmit
            ) :
                _buttonEntity(buttonEntity),
                _inputFieldEntity(inputFieldEntity),
                _onSubmit(onSubmit)
            {}

            void func(InputKeyName key, int scancode, InputAction action, int mods)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                UIElemState* pButtonUIState = (UIElemState*)pScene->getComponent(_buttonEntity, ComponentType::PK_UIELEM_STATE);
                UIElemState* pInputFieldUIState = (UIElemState*)pScene->getComponent(_inputFieldEntity, ComponentType::PK_UIELEM_STATE);
                TextRenderable* pContentText = (TextRenderable*)pScene->getComponentInChildren(
                    _buttonEntity,
                    ComponentType::PK_RENDERABLE_TEXT
                );

                if (!(pButtonUIState->isActive() && pInputFieldUIState->isActive()))
                    return;
                // NOTE: Not sure can this be used?
                bool inputModeActive = pButtonUIState->selected;
                if (inputModeActive)
                {
                    // <#M_DANGER> Not sure how badly this 'll end up fucking something...
                    std::string& txt = pContentText->accessStr();

                    if (action != PK_INPUT_RELEASE)
                    {
                        // Erasing front text if backspace
                        if (key == PK_INPUT_KEY_BACKSPACE)
                        {
                            if (!txt.empty())
                                txt.pop_back();
                        }
                        // Default submit behaviour..
                        if (key == PK_INPUT_KEY_ENTER)
                        {
                            if (_onSubmit)
                                _onSubmit->onSubmit(txt);

                            if (pInputFieldUIState->clearOnSubmit)
                                txt.clear();

                            pButtonUIState->selected = false;
                        }
                    }
                }
            }
        };


        class InputFieldCharInputEvent : public CharInputEvent
        {
        private:
            entityID_t _buttonEntity = NULL_ENTITY_ID;

        public:
            InputFieldCharInputEvent(
                entityID_t buttonEntity
            ) :
                _buttonEntity(buttonEntity)
            {}

            void func(unsigned int codepoint)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                UIElemState* pButtonUIState = (UIElemState*)pScene->getComponent(_buttonEntity, ComponentType::PK_UIELEM_STATE);
                TextRenderable* pContentText = (TextRenderable*)pScene->getComponentInChildren(
                    _buttonEntity,
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
                    std::string& txt = pContentText->accessStr();
                    txt.push_back(typedChar);
                }
            }
        };


        entityID_t create_image(
            HorizontalConstraintType horizontalType, float horizontalVal,
            VerticalConstraintType verticalType, float verticalVal,
            float width, float height,
            Texture* texture,
            vec3 color,
            vec4 borderColor,
            float borderThickness,
            Texture_new* pTexture,
            vec4 textureCropping
        )
        {
	    Application* app = Application::get();
	    Scene* currentScene = app->accessCurrentScene();

	    entityID_t entityID = currentScene->createEntity();
            currentScene->createTransform(entityID, { 0,0 }, { width, height });
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
            Transform* pTransform = currentScene->createTransform(entityID, { 0,0 }, { 1, 1 });

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


        entityID_t create_button(
            std::string txt, const Font& font,
            HorizontalConstraintType horizontalType, float horizontalVal,
            VerticalConstraintType verticalType, float verticalVal,
            float width, float height,
            OnClickEvent* onClick,
            bool selectable,
            vec3 color,
            vec4 borderColor,
            float borderThickness,
            Texture_new* pTexture,
            vec4 textureCropping
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();
            InputManager* inputManager = Application::get()->accessInputManager();

            entityID_t buttonEntity = currentScene->createEntity();
            entityID_t imgEntity = create_image(
                horizontalType, horizontalVal,
                verticalType, verticalVal,
                width, height,
                nullptr,
                color,
                borderColor,
                borderThickness,
                pTexture,
                textureCropping
            );

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
                verticalType, verticalVal
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

            // Create new UIElem state if not explicitly specified already
            UIElemState* pUIElemState = currentScene->createUIElemState(buttonEntity);
            pUIElemState->selectable = selectable;

            vec3 highlightColor(0.5f, 0.5f, 0.5f);
            vec3 selectedColor(0.3f, 0.3f, 0.3f);
            inputManager->addMouseButtonEvent(
                new ButtonMouseButtonEvent(
                    buttonEntity,
                    imgEntity,
                    selectedColor,
                    onClick
                )
            );
            inputManager->addCursorPosEvent(
                new ButtonMouseCursorPosEvent(
                    buttonEntity,
                    imgEntity,
                    highlightColor
                )
            );
            return buttonEntity;
        }


        #define PK_INPUTFIELD_DEFAULT_HEIGHT 21
        std::pair<entityID_t, TextRenderable*> create_input_field(
            std::string infoTxt, const Font& font,
            HorizontalConstraintType horizontalType, float horizontalVal,
            VerticalConstraintType verticalType, float verticalVal,
            int width,
            InputFieldOnSubmitEvent* onSubmitEvent,
            bool clearOnSubmit
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();
            InputManager* inputManager = Application::get()->accessInputManager();

            entityID_t inputFieldEntity = currentScene->createEntity();

            UIElemState* pUIElemState = currentScene->createUIElemState(inputFieldEntity);
            pUIElemState->selectable = true;
            pUIElemState->clearOnSubmit = clearOnSubmit;

            // Create button (*Override the button's UIElemState)
            entityID_t buttonEntity = create_button(
                "", // txt
                font,
                horizontalType, horizontalVal, // horiz. constraint
                verticalType, verticalVal, // vert. constraint
                (float)width, font.getPixelSize() + 4, // scale
                nullptr, // onclick
                true, // selectable
                { 0.05f, 0.05f, 0.05f }, // color
                { 0.1f, 0.1f, 0.1f, 1.0f }, // border color
                2, // border thickness
                nullptr, // texture
                {0, 0, 1, 1} // cropping
            );

            // figure out text's size
            float infoTxtDisplacement = font.getPixelSize();
            float pos = 0.0f;
            for (char c : infoTxt)
            {
                const FontGlyphData * const glyph = font.getGlyph(c);
                if (glyph)
                {
                    infoTxtDisplacement = pos + glyph->bearingX;
                    pos += ((float)(glyph->advance >> 6));
                }
            }
            // Also add a little gap..
            infoTxtDisplacement += font.getPixelSize();

            // Create info txt
            uint32_t infoTxtEntity = create_text(
                infoTxt, font,
                horizontalType, horizontalVal - infoTxtDisplacement, // *Add displacement to info text, so its to the right of the box
                verticalType, verticalVal
            ).first;

            currentScene->addChild(inputFieldEntity, buttonEntity);
            currentScene->addChild(inputFieldEntity, infoTxtEntity);

            // NOTE: button's text component is used as the InputField's "content text"
            TextRenderable* contentText = (TextRenderable*)currentScene->getComponentInChildren(
                buttonEntity,
                ComponentType::PK_RENDERABLE_TEXT
            );

            // TESTING
            Transform* buttonTransform = (Transform*)currentScene->getComponent(
                buttonEntity,
                ComponentType::PK_TRANSFORM
            );
            buttonTransform->accessTransformationMatrix()[1 + 3 * 4] -= 4;

            inputManager->addKeyEvent(
                new InputFieldKeyEvent(
                    buttonEntity,
                    inputFieldEntity,
                    onSubmitEvent
                )
            );
            inputManager->addCharInputEvent(
                new InputFieldCharInputEvent(
                    buttonEntity
                )
            );

            return std::make_pair(inputFieldEntity, contentText);
        }
    }
}
