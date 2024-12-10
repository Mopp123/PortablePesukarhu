#include "Scrollbar.h"
#include "Panel.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    namespace ui
    {
        static void scroll_ui_element(vec2 slotScale, float slotPadding, GUIElement* pElement, bool up)
        {
            // This attempts to gather all constraints depending on element type and scroll up or down
            // NOTE: Dangerous as fuck atm...
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

            // Alter scrolling depending on constraint type
            // *get first constraint since all constraints are required to have same type in panel
            VerticalConstraintType verticalConstraintType = constraints[0]->verticalType;

            float scrollAmount = -(slotScale.y + slotPadding);
            if (verticalConstraintType == VerticalConstraintType::PIXEL_CENTER_VERTICAL)
                scrollAmount = slotScale.y + slotPadding;

            if (up)
                scrollAmount *= -1.0f;

            for (ConstraintData* pConstraint : constraints)
            {
                // NOTE: depends on constraint type!
                pConstraint->verticalValue += scrollAmount;
            }
        }


        Scrollbar::OnClickUp::OnClickUp(Scrollbar* pScrollbar, Panel* pPanel) :
            _pScrollbar(pScrollbar),
            _pPanel(pPanel)
        {
        }

        void Scrollbar::OnClickUp::onClick(InputMouseButtonName button)
        {
            if (button == InputMouseButtonName::PK_INPUT_MOUSE_LEFT)
            {
                int visibleSlotCount = _pPanel->getVisibleVerticalSlots();
                if (_pScrollbar->_scrollPos - 1 >= 0)
                {
                    const int scrollPos = _pScrollbar->_scrollPos;
                    std::vector<GUIElement*> elements = _pPanel->_elements;

                    if (visibleSlotCount - scrollPos >= 0)
                    {
                        elements[scrollPos - 1]->setActive(true);
                        elements[visibleSlotCount + (scrollPos - 1)]->setActive(false);

                        // change all visible element's visual pos
                        for (GUIElement* pElement : elements)
                        {
                            scroll_ui_element(
                                _pPanel->_slotScale,
                                _pPanel->_slotPadding,
                                pElement,
                                true
                            );
                        }

                        --_pScrollbar->_scrollPos;
                    }
                }
            }
        }


        Scrollbar::OnClickDown::OnClickDown(Scrollbar* pScrollbar, Panel* pPanel) :
            _pScrollbar(pScrollbar),
            _pPanel(pPanel)
        {
        }

        void Scrollbar::OnClickDown::onClick(InputMouseButtonName button)
        {
            if (button == InputMouseButtonName::PK_INPUT_MOUSE_LEFT)
            {
                int visibleSlotCount = _pPanel->getVisibleVerticalSlots();
                if (_pScrollbar->_scrollPos + 1 <= visibleSlotCount)
                {
                    const int scrollPos = _pScrollbar->_scrollPos;
                    std::vector<GUIElement*> elements = _pPanel->_elements;

                    if (visibleSlotCount + scrollPos < elements.size())
                    {
                        elements[scrollPos]->setActive(false);
                        elements[visibleSlotCount + scrollPos]->setActive(true);

                        // change all visible element's visual pos
                        for (GUIElement* pElement : elements)
                        {
                            scroll_ui_element(
                                _pPanel->_slotScale,
                                _pPanel->_slotPadding,
                                pElement,
                                false
                            );
                        }

                        ++_pScrollbar->_scrollPos;
                    }
                }
            }
        }


        Scrollbar::Scrollbar(Panel* pPanel, Font* pFont) :
            _pPanel(pPanel)
        {
            const float defaultScrollbarWidth = 20.0f;
            float scrollbarHeight = _pPanel->_scale.y - _pPanel->_offsetFromPanel.y;
            vec2 buttonScale(defaultScrollbarWidth, defaultScrollbarWidth);

            GUIImage::ImgCreationProperties scrollbarBackgroundProperties;
            scrollbarBackgroundProperties.constraintProperties = _pPanel->_constraintProperties;
            float backgroundVerticalConstraintVal = _pPanel->_offsetFromPanel.y + buttonScale.y;
            if (_pPanel->_constraintProperties.verticalType == VerticalConstraintType::PIXEL_CENTER_VERTICAL)
                backgroundVerticalConstraintVal = -(_pPanel->_offsetFromPanel.y + buttonScale.y);

            scrollbarBackgroundProperties.constraintProperties.horizontalValue += _pPanel->_scale.x - defaultScrollbarWidth - _pPanel->_offsetFromPanel.x;
            scrollbarBackgroundProperties.constraintProperties.verticalValue += backgroundVerticalConstraintVal;

            scrollbarBackgroundProperties.width = defaultScrollbarWidth;
            scrollbarBackgroundProperties.height = scrollbarHeight - buttonScale.y * 2.0f;
            scrollbarBackgroundProperties.color = _pPanel->_color;
            scrollbarBackgroundProperties.filter = GUIFilterType::GUI_FILTER_TYPE_ENGRAVE;

            _pBackgroundImg = new GUIImage(scrollbarBackgroundProperties);

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
            float upVerticalConstraintVal = _pPanel->_offsetFromPanel.y;
            if (upButtonConstraintProperties.verticalType == VerticalConstraintType::PIXEL_CENTER_VERTICAL)
                upVerticalConstraintVal = -_pPanel->_offsetFromPanel.y;
            else if (upButtonConstraintProperties.verticalType == VerticalConstraintType::PIXEL_BOTTOM)
                upVerticalConstraintVal = _pPanel->_scale.y - (_pPanel->_offsetFromPanel.y + buttonScale.y);

            upButtonConstraintProperties.horizontalValue += _pPanel->_scale.x - defaultScrollbarWidth - _pPanel->_offsetFromPanel.x;
            upButtonConstraintProperties.verticalValue += upVerticalConstraintVal;

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
                new OnClickUp(this, _pPanel),
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
            float downVerticalConstraintVal = _pPanel->_scale.y - buttonScale.y - offsetFromBottom;
            if (upButtonConstraintProperties.verticalType == VerticalConstraintType::PIXEL_CENTER_VERTICAL)
                downVerticalConstraintVal = -_pPanel->_scale.y + buttonScale.y + offsetFromBottom;
            else if (upButtonConstraintProperties.verticalType == VerticalConstraintType::PIXEL_BOTTOM)
                downVerticalConstraintVal = _pPanel->_offsetFromPanel.y;

            downButtonConstraintProperties.horizontalValue += _pPanel->_scale.x - defaultScrollbarWidth - _pPanel->_offsetFromPanel.x;
            downButtonConstraintProperties.verticalValue += downVerticalConstraintVal;

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
                new OnClickDown(this, _pPanel),
                false, // selectable
                buttonColor,
                { 1, 1, 1 }, // text color
                { 1, 1, 1 }, // text highlight color
                buttonHighlightColor,
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS,
                pDefaultUITexture,
                downTexCropping
            );
        }

        Scrollbar::~Scrollbar()
        {
            delete _pBackgroundImg;
            delete _pUpButton;
            delete _pDownButton;
        }
    }
}
