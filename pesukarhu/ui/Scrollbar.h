#pragma once

#include "GUIButton.h"
#include "pesukarhu/core/input/InputEvent.h"


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
                OnClickUp(Scrollbar* pScrollbar) : _pScrollbar(pScrollbar) {}
                virtual void onClick(InputMouseButtonName button);
            };

            class OnClickDown : public GUIButton::OnClickEvent
            {
            public:
                Scrollbar* _pScrollbar = nullptr;
                OnClickDown(Scrollbar* pScrollbar) : _pScrollbar(pScrollbar) {}
                virtual void onClick(InputMouseButtonName button);
            };

            class IndicatorMouseButtonEvent : public MouseButtonEvent
            {
            public:
                Scrollbar* _pScrollbar = nullptr;
                IndicatorMouseButtonEvent(Scrollbar* pScrollbar) : _pScrollbar(pScrollbar) {}
		        virtual void func(InputMouseButtonName button, InputAction action, int mods);
            };

            class IndicatorCursorPosEvent : public CursorPosEvent
            {
            public:
                Scrollbar* _pScrollbar = nullptr;
                int _prevY = 0;
                IndicatorCursorPosEvent(Scrollbar* pScrollbar) : _pScrollbar(pScrollbar) {}
		        virtual void func(int x, int y);
            };

            class ScrollbarScrollEvent : public ScrollEvent
            {
            public:
                Scrollbar* _pScrollbar = nullptr;
                ScrollbarScrollEvent(Scrollbar* pScrollbar) : _pScrollbar(pScrollbar) {}
		        virtual void func(double xOffset, double yOffset);
            };

            Panel* _pPanel = nullptr;

            GUIImage* _pBackgroundImg = nullptr;
            GUIImage* _pScrollPosImg = nullptr;
            GUIButton* _pUpButton = nullptr;
            GUIButton* _pDownButton = nullptr;

            int _scrollPos = 0;
            bool _enableCursorScroll = false;

            float _dragAmount = 0.0f;

        public:
            Scrollbar(Panel* pPanel, Font* pFont, float topBarHeight);
            ~Scrollbar();
            void updateScrollIndicator();

            void setActive(bool arg);

            void reset();

        private:
            void scrollCursor(int dragBeginY, int cursorY);
            void scrollElement(GUIElement* pElement, bool up);
            // *On rewind we want to reset all positions but set all inactive to make reset work as expected..
            void scrollElements(bool up, bool rewind = false);
        };
    }
}
