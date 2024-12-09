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


        Checkbox::Checkbox(const Checkbox& other) :
            _entity(other._entity),
            _checkedStatusIndicator(other._checkedStatusIndicator)
        {
        }

        void Checkbox::create(
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

            _entity = pScene->createEntity();
            UIElemState* pUIElemState = UIElemState::create(_entity);

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
            _background.create(backgroundImgProperties);
            entityID_t backgroundImgEntity = _background.getEntity();

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
            _checkedStatusIndicator.create(checkedImgProperties);
            entityID_t checkedImgEntity = _checkedStatusIndicator.getEntity();

            // Create info txt
            ConstraintProperties infoTxtConstraintProperties = constraintProperties;
            infoTxtConstraintProperties.horizontalValue += infoDisplacement;
            _infoText.create(
                infoTxt, *pFont, // NOTE: DANGER! TODO: make all funcs here take font as ptr instead of ref!
                infoTxtConstraintProperties,
                textColor
            );
            entityID_t infoTextEntity = _infoText.getEntity();

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

        void Checkbox::setActive(bool arg)
        {
            _background.setActive(arg);
            _checkedStatusIndicator.setActive(arg);
            _infoText.setActive(arg);
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
            GUIRenderable* pCheckedStatusRenderable = _checkedStatusIndicator.getRenderable();
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
                    "No valid GUIRenderable found for checkbox checked status img entity: " + std::to_string(_checkedStatusIndicator.getEntity()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            pCheckboxState->checked = arg;
            pCheckedStatusRenderable->setActive(arg);
        }
    }
}
