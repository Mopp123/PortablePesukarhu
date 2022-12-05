#include "UIFactories.h"
#include "../../../core/Application.h"
#include "../../components/renderable/GUIRenderable.h"


namespace pk
{
    namespace ui
    {
        class ButtonMouseButtonEvent : public MouseButtonEvent
        {
        private:
            //BUTTON_STATE_INFO* _pBtnInfo = nullptr;
            UIElemState& _stateRef;
            OnClickEvent* _onClick = nullptr;
        public:
            ButtonMouseButtonEvent(UIElemState& elemStateRef, OnClickEvent* onClick) : 
                _stateRef(elemStateRef),
                _onClick(onClick)
            {}
            virtual void func(InputMouseButtonName button, InputAction action, int mods)
            {
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
                            if (_stateRef.selected)
                                Debug::log("___TEST___BUTTON SELECTED!");
                            else
                                Debug::log("___TEST___BUTTON DESELECTED!");
                            // TODO: Set highlighted!
                        }

                        if (_onClick)
                            _onClick->onClick(button);

                        _stateRef.state = 0;
                    }
                }
                else
                {
                    if (_stateRef.selectable)
                    {
                        _stateRef.selected = false;
                        // TODO: Set NOT highlighted!
                    }

                    _stateRef.state = 0;
                }
            }
        };

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
                
                if (x >= xPos && x <= xPos + width && y <= yPos && y >= yPos - height)
                {
                    _pImg->color = _highlightColor;
                    _stateRef.mouseOver = true;
                }
                else
                {
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


        uint32_t create_image(
            ConstraintType horizontalType, float horizontalVal, 
            ConstraintType verticalType, float verticalVal, 
            float width, float height,
            bool drawBorder,
            vec3 color
        )
        {
	    Application* app = Application::get();
	    Scene* currentScene = app->accessCurrentScene();

	    uint32_t entity = currentScene->createEntity();

	    Transform* transform = new Transform({ 0,0 }, { width, height });
	    GUIRenderable* renderable = new GUIRenderable;
            renderable->drawBorder = drawBorder;
            renderable->color = color;

	    currentScene->addComponent(entity, transform);
	    currentScene->addComponent(entity, renderable);
            
            currentScene->addSystem(new Constraint(transform, horizontalType, horizontalVal));
            currentScene->addSystem(new Constraint(transform, verticalType, verticalVal));

            return entity;
        }


        std::pair<uint32_t, TextRenderable*> create_text(
            const std::string& str, 
            ConstraintType horizontalType, float horizontalVal, 
            ConstraintType verticalType, float verticalVal, 
            bool bold
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();

            uint32_t entity = currentScene->createEntity();

            Transform* transform = new Transform({ 0,0 }, { 1,1 });
            TextRenderable* renderable = new TextRenderable(str, bold);

            currentScene->addComponent(entity, transform);
            currentScene->addComponent(entity, renderable);

            currentScene->addSystem(new Constraint(transform, horizontalType, horizontalVal));
            currentScene->addSystem(new Constraint(transform, verticalType, verticalVal));

            return std::make_pair(entity, renderable);
        }


        uint32_t create_button(
            std::string txt,
            ConstraintType horizontalType, float horizontalVal, 
            ConstraintType verticalType, float verticalVal, 
            float width, float height,
            OnClickEvent* onClick,
            bool selectable,
            int txtDisplacementX,
            int txtDisplacementY,
            UIElemState* pUIElemState,
            vec3 color
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();
            InputManager* inputManager = Application::get()->accessInputManager();
            
            uint32_t buttonEntity = currentScene->createEntity();
            uint32_t imgEntity = create_image(
                horizontalType, horizontalVal,
                verticalType, verticalVal,
                width, height,
                true,
                color
            );

            // Add txt displacement
            if (horizontalType == ConstraintType::PIXEL_LEFT || 
                horizontalType == ConstraintType::PIXEL_CENTER_HORIZONTAL)
                horizontalVal += (float)txtDisplacementX;
            else if (horizontalType == ConstraintType::PIXEL_RIGHT)
                horizontalVal -= (float)txtDisplacementX;

            if (verticalType == ConstraintType::PIXEL_TOP)
                verticalVal += (float)txtDisplacementY;
            else if (horizontalType == ConstraintType::PIXEL_BOTTOM ||
                verticalType == ConstraintType::PIXEL_CENTER_VERTICAL)
                verticalVal -= (float)txtDisplacementY;

            uint32_t txtEntity = create_text(
                txt, 
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

            // ... not sure why i did this???
            txtTransform->accessTransformationMatrix()[0 + 0 * 4] = width;
            txtTransform->accessTransformationMatrix()[1 + 1 * 4] = height;

            // Crente new UIElem state if not explicitly specified already
            UIElemState* uiElemState = pUIElemState;
            if (!uiElemState)
            {
                uiElemState = new UIElemState;
                currentScene->addComponent(buttonEntity, uiElemState);
            }

            inputManager->addMouseButtonEvent(new ButtonMouseButtonEvent(*uiElemState, onClick));
            inputManager->addCursorPosEvent(
                new ButtonMouseCursorPosEvent(
                    imgRenderable, 
                    imgTransform,
                    { 0.5f, 0.5f, 0.5f },
                    *uiElemState
                )
            );
            return buttonEntity;
        }


        #define PK_INPUTFIELD_DEFAULT_HEIGHT 16
        std::pair<uint32_t, TextRenderable*> create_input_field(
            std::string infoTxt,
            ConstraintType horizontalType, float horizontalVal, 
            ConstraintType verticalType, float verticalVal, 
            int width,
            InputFieldOnSubmitEvent* onSubmitEvent,
            bool clearOnSubmit
        )
        {
            Scene* currentScene = Application::get()->accessCurrentScene();
            InputManager* inputManager = Application::get()->accessInputManager();

            uint32_t inputFieldEntity = currentScene->createEntity();
            
            UIElemState* uiElemState = new UIElemState;
            uiElemState->selectable = true;
            currentScene->addComponent(inputFieldEntity, uiElemState);
            
            // Create button (*Override the button's UIElemState)
            uint32_t buttonEntity = create_button(
                "",
                horizontalType, horizontalVal, 
                verticalType, verticalVal, 
                (float)width, PK_INPUTFIELD_DEFAULT_HEIGHT,
                nullptr,
                true,
                0, 0,
                uiElemState,
                { 0.05f, 0.05f, 0.05f}
            );

            float infoTxtDisplacement = width;
            if (horizontalType == ConstraintType::PIXEL_RIGHT)
                infoTxtDisplacement = -width;
            // Create info txt
            uint32_t infoTxtEntity = create_text(
                infoTxt, 
                horizontalType, horizontalVal + infoTxtDisplacement, // *Add displacement to info text, so its to the right of the box
                verticalType, verticalVal
            ).first;

            currentScene->addChild(inputFieldEntity, buttonEntity);
            currentScene->addChild(inputFieldEntity, infoTxtEntity);

            // NOTE: button's text component is used as the InputField's "content text"
            TextRenderable* contentText = (TextRenderable*)currentScene->getComponentInChildren(
                buttonEntity, 
                ComponentType::PK_RENDERABLE_TEXT
            );
            
            inputManager->addKeyEvent(new InputFieldKeyEvent(*uiElemState, contentText, onSubmitEvent));
            inputManager->addCharInputEvent(new InputFieldCharInputEvent(*uiElemState, contentText));
            
            // Adjust the info text's transform a bit, so it doesnt look fucked..
            Transform* infoTextTransform = (Transform*)currentScene->getComponent(
                infoTxtEntity, 
                ComponentType::PK_TRANSFORM
            );
            infoTextTransform->accessTransformationMatrix()[0 + 0 * 4] = width;
            infoTextTransform->accessTransformationMatrix()[1 + 1 * 4] = PK_INPUTFIELD_DEFAULT_HEIGHT;

            return std::make_pair(inputFieldEntity, contentText);
        }
    }
}
