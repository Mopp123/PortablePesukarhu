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
            GUIRenderable* _pImg = nullptr;
            vec3 _originalColor = vec3(0, 0, 0);
            vec3 _highlightColor = vec3(1.0f, 1.0f, 1.0f);
            UIElemState& _stateRef;
            OnClickEvent* _onClick = nullptr;
        public:
            ButtonMouseButtonEvent(
                GUIRenderable* pImg,
                vec3 highlightColor,
                UIElemState& elemStateRef,
                OnClickEvent* onClick
            ) :
                _pImg(pImg),
                _highlightColor(highlightColor),
                _stateRef(elemStateRef),
                _onClick(onClick)
            {
                _originalColor = _pImg->color;
            }
            // NOTE: ONLY TESTING THIS ATM!!!
            virtual ~ButtonMouseButtonEvent()
            {
                if (_onClick)
                {
                    Debug::log("___TEST___DELETED ON CLICK!");
                    delete _onClick;
                }
            }
            virtual void func(InputMouseButtonName button, InputAction action, int mods)
            {
                if (!_stateRef.isActive())
                    return;
                if (_stateRef.mouseOver)
                {
                    if (action == PK_INPUT_PRESS)
                        _stateRef.state = 1;
                    else if (_stateRef.state == 1 && action == PK_INPUT_RELEASE)
                        _stateRef.state = 2;

                    // state: 0 = "nothing"
                    // state: 1 = pressed inside button
                    // state: 2 = pressed and released inside button

                    if (_stateRef.state == 2)
                    {
                        if (_stateRef.selectable)
                        {
                            _stateRef.selected = !_stateRef.selected;
                            _pImg->color = _highlightColor;
                        }

                        if (_onClick)
                            _onClick->onClick(button);

                        _stateRef.state = 0;
                    }
                }
                else
                {
                    _stateRef.selected = false;
                    _pImg->color = _originalColor;

                    _stateRef.state = 0;
                }
            }
        };

        // TODO: Make cursor pos event for ALL GUI IMAGES
        // (and maybe txt as well) to track if cursor is on ui
        class ButtonMouseCursorPosEvent : public CursorPosEvent
        {
        private:
            GUIRenderable* _pImg = nullptr;
            Transform* _pTransform = nullptr;
            vec3 _originalColor = vec3(0, 0, 0);
            vec3 _highlightColor = vec3(1, 1, 1);

            UIElemState& _stateRef;

        public:
            ButtonMouseCursorPosEvent(
                GUIRenderable* pImg,
                Transform* pTransform,
                vec3 highlightColor,
                UIElemState& elemStateRef
            ) :
                _pImg(pImg),
                _pTransform(pTransform),
                _highlightColor(highlightColor),
                _originalColor(pImg->color),
                _stateRef(elemStateRef)
            {}

            virtual void func(int x, int y)
            {
                mat4& tMat = _pTransform->accessTransformationMatrix();
                float width = tMat[0 + 0 * 4];
                float height = tMat[1 + 1 * 4];

                float xPos = tMat[0 + 3 * 4];
                float yPos = tMat[1 + 3 * 4];
                // *ignore highlight coloring if selected
                // set or remove highlight coloring
                if (_stateRef.isActive() && x >= xPos && x <= xPos + width && y <= yPos && y >= yPos - height)
                {
                    // Make sure theres no overlap with other ui (layer vals check)
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

                    if (!_stateRef.selected)
                        _pImg->color = _highlightColor;
                    _stateRef.mouseOver = true;
                }
                else
                {
                    // Reset current selected layer if needed to..
                    if (UIRenderableComponent::get_current_selected_layer() == _pImg->getLayerVal())
                        UIRenderableComponent::set_current_selected_layer(0);

                    if (!_stateRef.selected)
                        _pImg->color = _originalColor;
                    _stateRef.mouseOver = false;
                }
            }
        };


        class InputFieldKeyEvent : public KeyEvent
        {
        private:
            UIElemState& _stateRef;
            TextRenderable* _pContentText = nullptr;
            InputFieldOnSubmitEvent* _onSubmit = nullptr;
        public:
            InputFieldKeyEvent(
                UIElemState& elemStateRef,
                TextRenderable* pContentText,
                InputFieldOnSubmitEvent* onSubmit
            ) :
                _stateRef(elemStateRef),
                _pContentText(pContentText),
                _onSubmit(onSubmit)
            {}

            void  func(InputKeyName key, int scancode, InputAction action, int mods)
            {
                if (!_stateRef.isActive())
                    return;
                // NOTE: Not sure can this be used?
                bool isActive = _stateRef.selected;
                if (isActive)
                {
                    // <#M_DANGER> Not sure how badly this 'll end up fucking something...
                    std::string& txt = _pContentText->accessStr();

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

                            if (_stateRef.clearOnSubmit)
                                txt.clear();

                            _stateRef.selected = false;
                        }
                    }
                }
            }
        };


        class InputFieldCharInputEvent : public CharInputEvent
        {
        private:
            UIElemState& _stateRef;
            TextRenderable* _pContentText = nullptr;
        public:
            InputFieldCharInputEvent(
                UIElemState& elemStateRef,
                TextRenderable* pContentText
            ) :
                _stateRef(elemStateRef),
                _pContentText(pContentText)
            {}

            void func(unsigned int codepoint)
            {
                if (!_stateRef.isActive())
                    return;
                bool isActive = _stateRef.selected;
                if (isActive)
                {
                    // <#M_DANGER>
                    // just for testing atm.. quite dumb way to do it like this..
                    unsigned char typedChar = (unsigned char)codepoint;
                    std::string& txt = _pContentText->accessStr();
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
            vec4 textureCropping,
            UIElemState* pUIElemState
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
            UIElemState* uiElemState = pUIElemState;
            if (!uiElemState)
                uiElemState = currentScene->createUIElemState(buttonEntity);

            uiElemState->selectable = selectable;

            vec3 highlightColor(0.5f, 0.5f, 0.5f);
            vec3 selectedColor(0.3f, 0.3f, 0.3f);
            inputManager->addMouseButtonEvent(new ButtonMouseButtonEvent(imgRenderable, selectedColor, *uiElemState, onClick));
            inputManager->addCursorPosEvent(
                new ButtonMouseCursorPosEvent(
                    imgRenderable,
                    imgTransform,
                    highlightColor,
                    *uiElemState
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
                {0, 0, 1, 1}, // cropping
                pUIElemState // ui elem state
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

            inputManager->addKeyEvent(new InputFieldKeyEvent(*pUIElemState, contentText, onSubmitEvent));
            inputManager->addCharInputEvent(new InputFieldCharInputEvent(*pUIElemState, contentText));

            return std::make_pair(inputFieldEntity, contentText);
        }
    }
}
