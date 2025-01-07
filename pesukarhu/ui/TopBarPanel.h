#pragma once

#include "Panel.h"
#include "GUIButton.h"


namespace pk
{
    namespace ui
    {
        class TopBarPanel : public Panel
        {
        protected:
            class OnClickClose : public GUIButton::OnClickEvent
            {
            private:
                TopBarPanel* _pPanel = nullptr;
            public:
                OnClickClose(TopBarPanel* pPanel) : _pPanel(pPanel) {}
                virtual void onClick(InputMouseButtonName button);
            };

            float _topBarHeight = 20.0f;

            GUIImage* _pTopBarImg = nullptr;
            GUIText* _pTopBarTitle = nullptr;
            GUIButton* _pCloseButton = nullptr;

        public:
            ~TopBarPanel();
            void initBase(
                Scene* pScene,
                Font* pFont,
                const std::string title,
                ConstraintProperties constraintProperties,
                const vec2& scale,
                LayoutFillType fillType, // NOTE: This was previously forced to be HORIZONTAL!
                vec2 slotScale = vec2(200.0f, 24.0f),
                bool scrollable = false,
                bool closeable = true // wether or not create close button to the top bar
            );

            void setLayer(int layer);

            // NOTE: Below closes and opens panels setting all panel's elements and its' background to correct state.
            // No clue why the fuck I didn't do this before and required overriding those open and close funcs!
            virtual void open_NEW();
            virtual void close_NEW();
            virtual void onOpen() {};
            virtual void onClose() {};

            // TODO: Replace these with the NEW above
            virtual void open() {};
            virtual void close() {};

            inline float getTopBarHeight() const { return _topBarHeight; }

        protected:
            void setComponentsActive(bool arg);
        };
    }
}
