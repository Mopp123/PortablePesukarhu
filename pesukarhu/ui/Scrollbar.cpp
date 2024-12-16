#include "Scrollbar.h"
#include "Panel.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    namespace ui
    {
        void Scrollbar::OnClickUp::onClick(InputMouseButtonName button)
        {
            if (button == InputMouseButtonName::PK_INPUT_MOUSE_LEFT)
                _pScrollbar->scrollElements(true);
        }


        void Scrollbar::OnClickDown::onClick(InputMouseButtonName button)
        {
            if (button == InputMouseButtonName::PK_INPUT_MOUSE_LEFT)
                _pScrollbar->scrollElements(false);
        }


		void Scrollbar::IndicatorMouseButtonEvent::func(InputMouseButtonName button, InputAction action, int mods)
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            UIElemState* pIndicatorUIElem = (UIElemState*)pScene->getComponent(
                _pScrollbar->_pScrollPosImg->getEntity(),
                ComponentType::PK_UIELEM_STATE
            );
            if (button == InputMouseButtonName::PK_INPUT_MOUSE_LEFT && action == InputAction::PK_INPUT_PRESS)
            {
                if (pIndicatorUIElem->mouseOver)
                    _pScrollbar->_enableCursorScroll = true;
            }
            if (button == InputMouseButtonName::PK_INPUT_MOUSE_LEFT && action == InputAction::PK_INPUT_RELEASE)
            {
                _pScrollbar->_enableCursorScroll = false;
                _pScrollbar->_dragAmount = 0.0f;
            }
        }


	    void Scrollbar::IndicatorCursorPosEvent::func(int x, int y)
        {
            if (_pScrollbar->_enableCursorScroll)
            {
                _pScrollbar->scrollCursor(_prevY, y);
            }
            _prevY = y;
        }


		void Scrollbar::ScrollbarScrollEvent::func(double xOffset, double yOffset)
        {
            Scene* pScene = Application::get()->accessCurrentScene();

            if (yOffset != 0.0)
            {
                UIElemState* pPanelElemState = (UIElemState*)pScene->getComponent(
                    _pScrollbar->_pPanel->getEntity(),
                    ComponentType::PK_UIELEM_STATE
                );
                if (pPanelElemState->mouseOver)
                    _pScrollbar->scrollElements(yOffset < 0.0);
            }
        }


        Scrollbar::Scrollbar(Panel* pPanel, Font* pFont, float topBarHeight) :
            _pPanel(pPanel)
        {
            const float defaultScrollbarWidth = 20.0f;
            float scrollbarHeight = _pPanel->_scale.y + _pPanel->_offsetFromPanel.y;
            vec2 buttonScale(defaultScrollbarWidth, defaultScrollbarWidth);

            // Create background img
            GUIImage::ImgCreationProperties scrollbarBackgroundProperties;
            scrollbarBackgroundProperties.constraintProperties = _pPanel->_constraintProperties;

            float backgroundVerticalDisplacement = _pPanel->_offsetFromPanel.y - buttonScale.y;
            scrollbarBackgroundProperties.constraintProperties.horizontalValue += _pPanel->_scale.x - defaultScrollbarWidth - _pPanel->_offsetFromPanel.x;
            scrollbarBackgroundProperties.constraintProperties.verticalValue += backgroundVerticalDisplacement;

            scrollbarBackgroundProperties.width = defaultScrollbarWidth;
            scrollbarBackgroundProperties.height = scrollbarHeight - buttonScale.y * 2.0f;
            scrollbarBackgroundProperties.color = _pPanel->_color;
            scrollbarBackgroundProperties.filter = GUIFilterType::GUI_FILTER_TYPE_ENGRAVE;

            _pBackgroundImg = new GUIImage(scrollbarBackgroundProperties);

            // Create scroll pos indicator img
            // Figure out indicator's height depending on panel's out of bounds elements
            float outOfBoundsElements = std::max((float)_pPanel->_elements.size() - (float)_pPanel->getVisibleVerticalSlots(), 0.0f);
            const float indicatorHeight = scrollbarBackgroundProperties.height - (_pPanel->_slotScale.y * outOfBoundsElements);
            GUIImage::ImgCreationProperties scrollPosImgProperties;
            scrollPosImgProperties.constraintProperties = _pPanel->_constraintProperties;

            scrollPosImgProperties.constraintProperties.horizontalValue += _pPanel->_scale.x - defaultScrollbarWidth - _pPanel->_offsetFromPanel.x;
            scrollPosImgProperties.constraintProperties.verticalValue += backgroundVerticalDisplacement;

            scrollPosImgProperties.width = defaultScrollbarWidth;
            //scrollPosImgProperties.height = scrollbarHeight - buttonScale.y * 2.0f;
            scrollPosImgProperties.height = indicatorHeight;
            scrollPosImgProperties.color = _pPanel->_color;
            scrollPosImgProperties.highlightColor = scrollPosImgProperties.color * 1.25f;
            scrollPosImgProperties.useHighlightColor = true;
            scrollPosImgProperties.filter = GUIFilterType::GUI_FILTER_TYPE_EMBOSS;

            _pScrollPosImg = new GUIImage(scrollPosImgProperties);

            // Create up and down buttons
            ResourceManager& resManager = Application::get()->getResourceManager();
            Texture* pDefaultUITexture = resManager.getDefaultUITexture();
            ImageData* pUITexImgData = (ImageData*)resManager.getResource(pDefaultUITexture->getImgResourceID());

            vec3 buttonColor(0.8f, 0.8f, 0.8f);
            vec3 buttonHighlightColor(1.0f, 1.0f, 1.0f);
            float imgTotalWidth = 32.0f;
            float cellWidth = 16.0f;

            // Up button
            ConstraintProperties upButtonConstraintProperties = _pPanel->_constraintProperties;
            upButtonConstraintProperties.horizontalValue += _pPanel->_scale.x - defaultScrollbarWidth - _pPanel->_offsetFromPanel.x;
            upButtonConstraintProperties.verticalValue += _pPanel->_offsetFromPanel.y;

            vec4 upTexCropping;
            upTexCropping.x = 0;
            upTexCropping.y = 1;
            upTexCropping.z = cellWidth / imgTotalWidth;
            upTexCropping.w = cellWidth / imgTotalWidth;

            _pUpButton = new GUIButton(
                "",
                *pFont,
                upButtonConstraintProperties,
                buttonScale.x,
                buttonScale.y,
                new OnClickUp(this),
                false, // selectable
                buttonColor,
                { 1, 1, 1 }, // text color
                { 1, 1, 1 }, // text highlight color
                buttonHighlightColor,
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS,
                pDefaultUITexture,
                upTexCropping
            );

            // Down button
            ConstraintProperties downButtonConstraintProperties = _pPanel->_constraintProperties;

            // make the down button not be exactly on the bottom line...
            const float offsetFromBottom = 4.0f;
            downButtonConstraintProperties.horizontalValue += _pPanel->_scale.x - defaultScrollbarWidth - _pPanel->_offsetFromPanel.x;
            float downVerticalConstraintDisplacement = -_pPanel->_scale.y + buttonScale.y + offsetFromBottom;
            downButtonConstraintProperties.verticalValue += downVerticalConstraintDisplacement;

            vec4 downTexCropping;
            downTexCropping.x = 1;
            downTexCropping.y = 1;
            downTexCropping.z = cellWidth / imgTotalWidth;
            downTexCropping.w = cellWidth / imgTotalWidth;

            _pDownButton = new GUIButton(
                "",
                *pFont,
                downButtonConstraintProperties,
                buttonScale.x,
                buttonScale.y,
                new OnClickDown(this),
                false, // selectable
                buttonColor,
                { 1, 1, 1 }, // text color
                { 1, 1, 1 }, // text highlight color
                buttonHighlightColor,
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS,
                pDefaultUITexture,
                downTexCropping
            );

            InputManager* pInputManager = Application::get()->accessInputManager();
            pInputManager->addMouseButtonEvent(new IndicatorMouseButtonEvent(this));
            pInputManager->addCursorPosEvent(new IndicatorCursorPosEvent(this));
            pInputManager->addScrollEvent(new ScrollbarScrollEvent(this));
        }

        Scrollbar::~Scrollbar()
        {
            delete _pBackgroundImg;
            delete _pScrollPosImg;
            delete _pUpButton;
            delete _pDownButton;
        }

        void Scrollbar::updateScrollIndicator()
        {
            // Figure out indicator's height depending on panel's out of bounds elements
            float visibleElements = (float)_pPanel->getVisibleVerticalSlots();
            float outOfBoundsElements = std::max((float)_pPanel->_elements.size() - visibleElements, 0.0f);

            Transform* pBackgroundTransform = _pBackgroundImg->getTransform();
            float backgroundHeight = pBackgroundTransform->getGlobalScale().y;
            // stepSize means how much single element occupies on the scrollbar
            float stepSize = backgroundHeight / std::max((float)_pPanel->_elements.size(), 1.0f);
            float indicatorHeight = backgroundHeight - (outOfBoundsElements * stepSize);

            ConstraintData* pBackgroundConstraint = _pBackgroundImg->getConstraint();
            ConstraintData* pIndicatorConstraintData = _pScrollPosImg->getConstraint();

            float indicatorVerticalVal = pBackgroundConstraint->verticalValue - (_scrollPos * stepSize);
            pIndicatorConstraintData->verticalValue = indicatorVerticalVal;
            pIndicatorConstraintData->horizontalValue = pBackgroundConstraint->horizontalValue;

            Transform* pIndicatorTransform = _pScrollPosImg->getTransform();
            vec3 indicatorScale = pIndicatorTransform->getGlobalScale();
            indicatorScale.y = indicatorHeight;
            pIndicatorTransform->setScale(indicatorScale);
        }

        void Scrollbar::setActive(bool arg)
        {
            _pBackgroundImg->setActive(arg);
            _pScrollPosImg->setActive(arg);
            _pUpButton->setActive(arg);
            _pDownButton->setActive(arg);

            reset();
        }

        void Scrollbar::reset()
        {
            for (int i = 0; i < _scrollPos; ++i)
                scrollElements(true, true);

            _scrollPos = 0;
            _enableCursorScroll = false;
        }

        void Scrollbar::scrollCursor(int prevY, int newY)
        {
            if (prevY == newY)
                return;

            Transform* pBackgroundTransform = _pBackgroundImg->getTransform();
            float backgroundHeight = pBackgroundTransform->getGlobalScale().y;
            // stepSize means how much single element occupies on the scrollbar
            float stepSize = backgroundHeight / std::max((float)_pPanel->_elements.size(), 1.0f);
            float fPrevY = (float)prevY;
            float fNewY = (float)newY;

            _dragAmount += std::abs(fNewY - fPrevY);
            if (_dragAmount >= stepSize)
            {
                int a = _dragAmount / stepSize;
                // NOTE: depends on constraint type
                bool scrollUp = fNewY > fPrevY;
                for (int i = 0; i < a; ++i)
                    scrollElements(scrollUp);

                _dragAmount = 0.0f;
            }
        }

        void Scrollbar::scrollElement(GUIElement* pElement, bool up)
        {
            // This attempts to gather all constraints depending on element type and scroll up or down
            // ...Danger?
            std::vector<ConstraintData*> constraints;
            if (pElement->getType() == GUIElementType::PK_GUI_ELEMENT_TYPE_TEXT)
            {
                GUIText* pText = (GUIText*)pElement;
                constraints.push_back(pText->getConstraint());
            }
            else if (pElement->getType() == GUIElementType::PK_GUI_ELEMENT_TYPE_IMAGE)
            {
                GUIImage* pImage = (GUIImage*)pElement;
                constraints.push_back(pImage->getConstraint());
            }
            else if (pElement->getType() == GUIElementType::PK_GUI_ELEMENT_TYPE_BUTTON)
            {
                GUIButton* pButton = (GUIButton*)pElement;
                constraints.push_back(pButton->getImage()->getConstraint());
                constraints.push_back(pButton->getText()->getConstraint());
            }
            else if (pElement->getType() == GUIElementType::PK_GUI_ELEMENT_TYPE_INPUT_FIELD)
            {
                InputField* pInputField = (InputField*)pElement;
                GUIButton* pButton = pInputField->getButton();
                constraints.push_back(pButton->getImage()->getConstraint());
                constraints.push_back(pButton->getText()->getConstraint());
                constraints.push_back(pInputField->getInfoText()->getConstraint());
            }
            else if (pElement->getType() == GUIElementType::PK_GUI_ELEMENT_TYPE_CHECKBOX)
            {
                Checkbox* pCheckbox = (Checkbox*)pElement;
                constraints.push_back(pCheckbox->getBackgroundImage()->getConstraint());
                constraints.push_back(pCheckbox->getStatusImage()->getConstraint());
                constraints.push_back(pCheckbox->getInfoText()->getConstraint());
            }
            if (constraints.empty())
            {
                Debug::log(
                    "@scroll_ui_element "
                    "Attempted to scroll panel elements but no constraints found!",
                    Debug::MessageType::PK_WARNING
                );
                return;
            }

            const vec2 slotScale = _pPanel->_slotScale;
            const float slotPadding = _pPanel->_slotPadding;
            float scrollAmount = slotScale.y + slotPadding;

            if (up)
                scrollAmount *= -1.0f;

            for (ConstraintData* pConstraint : constraints)
                pConstraint->verticalValue += scrollAmount;
        }

        void Scrollbar::scrollElements(bool up, bool rewind)
        {
            int visibleSlotCount = _pPanel->getVisibleVerticalSlots();
            std::vector<GUIElement*> elements = _pPanel->_elements;
            const size_t elemCount = elements.size();
            int showIndex = visibleSlotCount + _scrollPos;
            int hideIndex = _scrollPos;
            int newScrollPos = up ? _scrollPos - 1 : _scrollPos + 1;
            if (up)
            {
                showIndex = _scrollPos - 1;
                hideIndex = visibleSlotCount + (_scrollPos - 1);
            }

            if (showIndex >= 0 && hideIndex >= 0 && showIndex < elemCount && hideIndex < elemCount)
            {
                elements[showIndex]->setActive(rewind ? false : true);
                elements[hideIndex]->setActive(false);

                // change all visible element's visual pos
                for (GUIElement* pElement : elements)
                {
                    scrollElement(
                        pElement,
                        up
                    );
                }

                _scrollPos = newScrollPos;
                updateScrollIndicator();
            }
        }
    }
}
