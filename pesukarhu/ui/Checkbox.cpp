#include "Checkbox.h"
#include "UIUtils.h"
#include "GUIText.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    namespace ui
    {
        Checkbox::CheckboxMouseButtonEvent::CheckboxMouseButtonEvent(
            entityID_t checkboxEntity,
            entityID_t checkboxImgEntity,
            entityID_t checkStatusEntity
        ) :
            _checkboxEntity(checkboxEntity),
            _checkboxImgEntity(checkboxImgEntity),
            _checkStatusEntity(checkStatusEntity)
        {
        }

        void Checkbox::CheckboxMouseButtonEvent::func(
            InputMouseButtonName button,
            InputAction action,
            int mods
        )
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


        Checkbox::Checkbox(
            std::string infoTxt,
            const Font* pFont,
            ConstraintProperties constraintProperties,
            vec3 backgroundColor,
            vec3 backgroundHighlightColor,
            vec3 checkedColor, // The color of the thing indicating checked status
            vec3 textColor
        ) :
            GUIElement(GUIElementType::PK_GUI_ELEMENT_TYPE_CHECKBOX),
            _pFont(pFont)
        {
            Scene* pScene = Application::get()->accessCurrentScene();

            _entity = pScene->createEntity();
            UIElemState* pUIElemState = UIElemState::create(_entity);

            const float defaultWidth = pFont->getPixelSize();
            const float defaultHeight = pFont->getPixelSize();

            // -> No fucking idea what that magic 4 comes from.. but seems good..
            //  ->same as with inputField...
            float backgroundDisplacement = get_text_visual_width(infoTxt, pFont) + ((float)pFont->getPixelSize()) - 4;

            ConstraintProperties backgroundImgConstraintProperties = constraintProperties;
            backgroundImgConstraintProperties.horizontalValue += backgroundDisplacement;
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

            _pBackground = new GUIImage(backgroundImgProperties);
            entityID_t backgroundImgEntity = _pBackground->getEntity();

            _checkedImgMinification = 8.0f; // how many pixels smaller than outer box
            ConstraintProperties checkedImgConstraintProperties = backgroundImgConstraintProperties;
            checkedImgConstraintProperties.horizontalValue = backgroundImgConstraintProperties.horizontalValue + _checkedImgMinification * 0.5f;

            float checkedImgDisplacementY = _checkedImgMinification * 0.5f;

            checkedImgConstraintProperties.verticalValue = backgroundImgConstraintProperties.verticalValue - checkedImgDisplacementY;
            GUIImage::ImgCreationProperties checkedImgProperties =
            {
                checkedImgConstraintProperties,
                defaultWidth - _checkedImgMinification,
                defaultHeight - _checkedImgMinification,
                checkedColor,
                checkedColor,
                true,
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS,
                nullptr
            };
            _pCheckedStatusIndicator = new GUIImage(checkedImgProperties);
            entityID_t checkedImgEntity = _pCheckedStatusIndicator->getEntity();

            // Create info txt
            ConstraintProperties infoTxtConstraintProperties = constraintProperties;
            _pInfoText = new GUIText(
                infoTxt, *pFont, // NOTE: DANGER! TODO: make all funcs here take font as ptr instead of ref!
                infoTxtConstraintProperties,
                textColor
            );
            entityID_t infoTextEntity = _pInfoText->getEntity();

            pScene->addChild(_entity, backgroundImgEntity);
            pScene->addChild(_entity, checkedImgEntity);
            pScene->addChild(_entity, infoTextEntity);

            InputManager* pInputManager = Application::get()->accessInputManager();
            pInputManager->addMouseButtonEvent(
                new CheckboxMouseButtonEvent(
                    _entity,
                    backgroundImgEntity,
                    checkedImgEntity
                )
            );

            // Set unchecked by default
            setChecked(false);
        }

        Checkbox::~Checkbox()
        {
            if (_pBackground)
                delete _pBackground;
            if (_pCheckedStatusIndicator)
                delete _pCheckedStatusIndicator;
            if (_pInfoText)
                delete _pInfoText;
        }

        void Checkbox::setActive(bool arg)
        {
            _pBackground->setActive(arg);
            _pInfoText->setActive(arg);

            // If setting active
            // -> make sure checked indicator matches the UIElemState
            //  -> don't just set it active so its' visual state doesn't get out of sync of the
            //  UIElemState
            if (arg)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                const UIElemState* pState = (const UIElemState*)pScene->getComponent(
                    _entity,
                    ComponentType::PK_UIELEM_STATE
                );
                _pCheckedStatusIndicator->setActive(pState->checked);
            }
            else
            {
                _pCheckedStatusIndicator->setActive(arg);
            }
        }

        void Checkbox::setConstraintValues(float horizontal, float vertical)
        {
            float textVisualWidth = get_text_visual_width(_pInfoText->getInternalStr(), _pFont);
            float backgroundDisplacement = textVisualWidth + ((float)_pFont->getPixelSize()) - 4;
            float backgroundHorizontalValue = horizontal + backgroundDisplacement;
            _pBackground->setConstraintValues(backgroundHorizontalValue, vertical);

            _pCheckedStatusIndicator->setConstraintValues(
                backgroundHorizontalValue + _checkedImgMinification * 0.5f,
                vertical - _checkedImgMinification * 0.5f
            );

            _pInfoText->setConstraintValues(horizontal, vertical);
        }

        bool Checkbox::isChecked() const
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            const UIElemState* pState = (const UIElemState*)pScene->getComponent(
                _entity,
                ComponentType::PK_UIELEM_STATE
            );
            if (!pState)
            {
                Debug::log(
                    "@Checkbox::isChecked "
                    "No valid UIElemState found for entity: " + std::to_string(_entity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return false;
            }
            return pState->checked;
        }

        void Checkbox::setChecked(bool arg)
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            UIElemState* pCheckboxState = (UIElemState*)pScene->getComponent(
                _entity,
                ComponentType::PK_UIELEM_STATE
            );
            GUIRenderable* pCheckedStatusRenderable = _pCheckedStatusIndicator->getRenderable();
            if (!pCheckboxState)
            {
                Debug::log(
                    "@Checkbox::setChecked "
                    "No valid UIElemState found for checkbox entity: " + std::to_string(_entity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            if (!pCheckedStatusRenderable)
            {
                Debug::log(
                    "@Checkbox::setChecked "
                    "No valid GUIRenderable found for checkbox checked status img entity: " + std::to_string(_pCheckedStatusIndicator->getEntity()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            pCheckboxState->checked = arg;
            pCheckedStatusRenderable->setActive(arg);
        }
    }
}
