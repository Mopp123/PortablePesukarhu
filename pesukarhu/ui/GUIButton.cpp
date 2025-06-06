#include "GUIButton.h"
#include "GUIImage.h"
#include "GUIText.h"
#include "pesukarhu/core/Application.h"
#include <vector>


namespace pk
{
    namespace ui
    {
        GUIButton::ButtonMouseButtonEvent::ButtonMouseButtonEvent(
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
            GUIRenderable* pImg = (GUIRenderable*)pScene->getComponent(
                _imgEntity,
                ComponentType::PK_RENDERABLE_GUI
            );
            _originalColor = pImg->color;
        }

        GUIButton::ButtonMouseButtonEvent::~ButtonMouseButtonEvent()
        {
            if (_onClick)
                delete _onClick;
        }

        void GUIButton::ButtonMouseButtonEvent::func(
            InputMouseButtonName button,
            InputAction action,
            int mods
        )
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


        GUIButton::ButtonMouseCursorPosEvent::ButtonMouseCursorPosEvent(
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

        void GUIButton::ButtonMouseCursorPosEvent::func(int x, int y)
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

        GUIButton::GUIButton(
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
        ) :
            GUIElement(GUIElementType::PK_GUI_ELEMENT_TYPE_BUTTON)
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            InputManager* inputManager = Application::get()->accessInputManager();

            _entity = pScene->createEntity();

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

            _pImage = new GUIImage(imgProperties);
            entityID_t imgEntity = _pImage->getEntity();

            // Add txt displacement
            const int padding = 4;
            const int txtDisplacementX = padding;

            ConstraintProperties useConstraintProperties = constraintProperties;
            useConstraintProperties.horizontalValue += (float)txtDisplacementX;

            _pText = new GUIText(
                txt, font,
                useConstraintProperties,
                textColor
            );
            entityID_t txtEntity = _pText->getEntity();

            pScene->addChild(_entity, imgEntity);
            pScene->addChild(_entity, txtEntity);

            Transform* txtTransform = _pText->getTransform();
            // alter transform's scale to make work properly with constraint system
            txtTransform->accessTransformationMatrix()[0 + 0 * 4] = width;
            txtTransform->accessTransformationMatrix()[1 + 1 * 4] = height;

            UIElemState* pUIElemState = (UIElemState*)pScene->getComponent(
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
        }

        GUIButton::~GUIButton()
        {
            if (_pImage)
                delete _pImage;
            if (_pText)
                delete _pText;
        }

        void GUIButton::setActive(bool arg)
        {
            _pImage->setActive(arg);
            _pText->setActive(arg);
            Scene* pScene = Application::get()->accessCurrentScene();
            std::vector<Component*> components = pScene->getComponents(_entity);
            for (Component* pComponent : components)
                pComponent->setActive(arg);
        }
    }
}
