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

            GUIImage* _pTopBarImg = nullptr;
            GUIText* _pTopBarTitle = nullptr;

        public:
            ~TopBarPanel();
            void initBase(
                Scene* pScene,
                Font* pFont,
                const std::string title,
                ConstraintProperties constraintProperties,
                const vec2& scale,
                LayoutFillType fillType // NOTE: This was previously forced to be HORIZONTAL!
            );

            virtual void open() {};
            virtual void close() {};

        protected:
            void setComponentsActive(bool arg);
        };
    }
}
