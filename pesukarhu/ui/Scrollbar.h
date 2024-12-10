#pragma once

#include "GUIButton.h"


namespace pk
{
    namespace ui
    {
        class Panel;

        class Scrollbar
        {
        private:
            class OnClickUp : public GUIButton::OnClickEvent
            {
            public:
                Scrollbar* _pScrollbar = nullptr;
                Panel* _pPanel = nullptr;
                OnClickUp(Scrollbar* pScrollbar, Panel* pPanel);
                virtual void onClick(InputMouseButtonName button);
            };

            class OnClickDown : public GUIButton::OnClickEvent
            {
            public:
                Scrollbar* _pScrollbar = nullptr;
                Panel* _pPanel = nullptr;
                OnClickDown(Scrollbar* pScrollbar, Panel* pPanel);
                virtual void onClick(InputMouseButtonName button);
            };

            Panel* _pPanel = nullptr;

            GUIImage* _pBackgroundImg = nullptr;
            GUIButton* _pUpButton = nullptr;
            GUIButton* _pDownButton = nullptr;

            int _scrollPos = 0;

        public:
            Scrollbar(Panel* pPanel, Font* pFont);
            ~Scrollbar();
        };
    }
}
