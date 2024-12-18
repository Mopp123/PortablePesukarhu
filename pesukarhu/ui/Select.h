#pragma once

#include "GUIElement.h"
#include "GUIButton.h"
#include "GUIText.h"
#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/resources/Font.h"


namespace pk
{
    namespace ui
    {
        class Panel;


        struct SelectCreationProperties
        {
            std::string infoTxt;
            Font* pFont;
            ConstraintProperties constraintProperties;
            int width;
            std::vector<std::string> items;
            vec3 color = { 0.1f, 0.1f, 0.1f };
            vec3 textColor = { 1, 1, 1 };
            vec3 textHighlightColor = { 1, 1, 1 };
            vec3 backgroundHighlightColor = { 0.2f, 0.2f, 0.2f };
        };


        class Select : public GUIElement
        {
        private:
            class OnClickDisplaySelection : public GUIButton::OnClickEvent
            {
            public:
                Select* _pSelect = nullptr;
                OnClickDisplaySelection(Select* pSelect) : _pSelect(pSelect) {}
                virtual void onClick(InputMouseButtonName button);
            };

            class OnClickSelectItem : public GUIButton::OnClickEvent
            {
            public:
                Select* _pSelect = nullptr;
                unsigned int _itemIndex = 0;
                OnClickSelectItem(Select* pSelect, unsigned int itemIndex) : _pSelect(pSelect), _itemIndex(itemIndex) {}
                virtual void onClick(InputMouseButtonName button);
            };

            // Need mouse event to close if clicking outside the selection
	        class SelectMouseEvent : public MouseButtonEvent
	        {
	        public:
                Select* _pSelect = nullptr;
                SelectMouseEvent(Select* pSelect) : _pSelect(pSelect) {}
	        	virtual void func(InputMouseButtonName button, InputAction action, int mods);
	        };

            // Also close if esc key
	        class SelectKeyEvent : public KeyEvent
	        {
	        public:
                Select* _pSelect = nullptr;
                SelectKeyEvent(Select* pSelect) : _pSelect(pSelect) {}
	        	virtual void func(InputKeyName key, int scancode, InputAction action, int mods);
	        };

            GUIButton* _pButton = nullptr;
            GUIText* _pInfoText = nullptr;
            Panel* _pSelection = nullptr;

            bool _isOpen = false;
            unsigned int _selectedIndex = 0;

        public:
            Select(SelectCreationProperties creationProperties);
            Select(const Select& other) = delete;
            ~Select();

            void setActive(bool arg);

            void displaySelection(bool arg);
            void setSelectedItem(unsigned int itemIndex);
            std::string getSelectedStr() const;

            inline GUIButton* getButton() { return _pButton; }
            inline GUIText* getInfoText() { return _pInfoText; }
            inline Panel* getPanel() { return _pSelection; }

            inline unsigned int getSelectedIndex() const { return _selectedIndex; }
        };
    }
}
