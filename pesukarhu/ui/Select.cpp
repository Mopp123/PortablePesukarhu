#include "Select.h"
#include "pesukarhu/core/Application.h"
#include "Panel.h"


namespace pk
{
    namespace ui
    {
        void Select::OnClickDisplaySelection::onClick(InputMouseButtonName button)
        {
            if (button == InputMouseButtonName::PK_INPUT_MOUSE_LEFT)
                _pSelect->displaySelection(!_pSelect->_isOpen);
        }


        void Select::OnClickSelectItem::onClick(InputMouseButtonName button)
        {
            if (button == InputMouseButtonName::PK_INPUT_MOUSE_LEFT)
            {
                _pSelect->setSelectedItem(_itemIndex);
                _pSelect->displaySelection(false);
            }
        }


	    void Select::SelectMouseEvent::func(InputMouseButtonName button, InputAction action, int mods)
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            const UIElemState* pButtonImgUIState = (const UIElemState*)pScene->getComponent(
                _pSelect->_pButton->getImage()->getEntity(),
                ComponentType::PK_UIELEM_STATE
            );

            if (!_pSelect->_pSelection->isMouseOver() && !pButtonImgUIState->mouseOver)
                _pSelect->displaySelection(false);
        }


	    void Select::SelectKeyEvent::func(InputKeyName key, int scancode, InputAction action, int mods)
        {
            if (key == InputKeyName::PK_INPUT_KEY_ESCAPE)
                _pSelect->displaySelection(false);
        }


        Select::Select(SelectCreationProperties creationProperties) :
            GUIElement(GUIElementType::PK_GUI_ELEMENT_TYPE_SELECt)
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            _entity = pScene->createEntity();

            Font* pFont = creationProperties.pFont;
            // figure out text's size
            float infoTxtWidth = 0.0f;
            float pos = 0.0f;
            for (char c : creationProperties.infoTxt)
            {
                const FontGlyphData * const glyph = pFont->getGlyph(c);
                if (glyph)
                {
                    infoTxtWidth = pos + glyph->bearingX;
                    pos += ((float)(glyph->advance >> 6));
                }
            }
            // No fucking idea what that magic 4 comes from.. but seems good..
            float buttonDisplacementX = infoTxtWidth;
            float buttonDisplacementY = 0.0f;
            float infoDisplacement = 0;
            buttonDisplacementX += ((float)pFont->getPixelSize()) - 4;

            ConstraintProperties buttonConstraintProperties = creationProperties.constraintProperties;
            buttonConstraintProperties.horizontalValue += buttonDisplacementX;
            buttonConstraintProperties.verticalValue += buttonDisplacementY;

            // Create button
            const float buttonHeight = pFont->getPixelSize();
            _pButton = new GUIButton(
                "SELECT", // txt
                *pFont,
                buttonConstraintProperties,
                (float)creationProperties.width,
                buttonHeight,
                new OnClickDisplaySelection(this), // onclick
                false, // selectable
                creationProperties.color,
                creationProperties.textColor,
                creationProperties.textHighlightColor,
                creationProperties.backgroundHighlightColor,
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS,
                nullptr, // texture
                {0, 0, 1, 1} // cropping
            );
            entityID_t buttonRootEntity = _pButton->getEntity();
            entityID_t buttonImgEntity = _pButton->getImage()->getEntity();

            ConstraintProperties infoTxtConstraintProperties = creationProperties.constraintProperties;
            infoTxtConstraintProperties.horizontalValue += infoDisplacement;
            // Create info txt
            _pInfoText = new GUIText(
                creationProperties.infoTxt,
                *pFont,
                infoTxtConstraintProperties,
                creationProperties.textColor
            );
            entityID_t infoTextEntity = _pInfoText->getEntity();

            pScene->addChild(_entity, buttonRootEntity);
            pScene->addChild(_entity, infoTextEntity);

            // TESTING
            // NOTE: This should have not worked before... refers to non existent transform...
            Transform* buttonTransform = (Transform*)pScene->getComponent(
                buttonImgEntity,
                ComponentType::PK_TRANSFORM
            );
            buttonTransform->accessTransformationMatrix()[1 + 3 * 4] -= 4;

            ConstraintProperties selectionConstraintProperties = creationProperties.constraintProperties;
            selectionConstraintProperties.horizontalValue = buttonConstraintProperties.horizontalValue;
            selectionConstraintProperties.verticalValue = buttonConstraintProperties.verticalValue - buttonHeight;
            const float addSelectionWidth = 28.0f; // add so that selection slots don't overlap with scrollbar
            vec2 selectionScale(creationProperties.width + addSelectionWidth, 100);
            vec2 selectionSlotScale(creationProperties.width - 4, pFont->getPixelSize());
            _pSelection = new Panel;
            _pSelection->createDefault(
                pScene,
                pFont,
                selectionConstraintProperties,
                selectionScale,
                selectionSlotScale,
                Panel::LayoutFillType::VERTICAL,
                true,
                0.0f, // top bar height
                1 // use color index
            );

            const std::vector<std::string>& selectionItems = creationProperties.items;
            for (int i = 0; i < selectionItems.size(); ++i)
            {
                _pSelection->addDefaultButton(
                    selectionItems[i],
                    new OnClickSelectItem(this, i),
                    selectionSlotScale.x
                );
            }

            InputManager* pInputManager = Application::get()->accessInputManager();
            pInputManager->addMouseButtonEvent(new SelectMouseEvent(this));
            pInputManager->addKeyEvent(new SelectKeyEvent(this));

            setSelectedItem(0);
            displaySelection(false);
        }

        Select::~Select()
        {
            delete _pButton;
            delete _pInfoText;
            delete _pSelection;
        }

        void Select::setActive(bool arg)
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@Select::setActive "
                    "Select's entity was null. "
                    "Make sure Select was created successfully!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            _pButton->setActive(arg);
            _pInfoText->setActive(arg);
            Scene* pScene = Application::get()->accessCurrentScene();
            for (Component* pComponent : pScene->getComponents(_entity))
                pComponent->setActive(arg);

            _pSelection->setActive(arg);
        }

        void Select::displaySelection(bool arg)
        {
            _pSelection->setActive(arg);
            _isOpen = arg;
        }

        void Select::setSelectedItem(unsigned int itemIndex)
        {
            const std::vector<GUIElement*> elements = _pSelection->getElements();
            if (itemIndex >= elements.size())
            {
                Debug::log(
                    "@Select::setSelectedItem "
                    "itemIndex: " + std::to_string(itemIndex) + " outside the selection elements. "
                    "Selection element count: " + std::to_string(elements.size()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            GUIElement* pElement = elements[itemIndex];
            if (!pElement)
            {
                Debug::log(
                    "@Select::setSelectedItem "
                    "Selected GUIElement at index: " + std::to_string(itemIndex) + " was nullptr",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            if (pElement->getType() != GUIElementType::PK_GUI_ELEMENT_TYPE_BUTTON)
            {
                Debug::log(
                    "@Select::setSelectedItem "
                    "Selection element wasn't button",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            GUIButton* pElemButton = (GUIButton*)pElement;
            const std::string selectedStr = pElemButton->getText()->getInternalStr();
            _pButton->getText()->setInternalStr(selectedStr);
            _selectedIndex = itemIndex;
        }

        std::string Select::getSelectedStr() const
        {
            return _pButton->getText()->getInternalStr();
        }
    }
}
