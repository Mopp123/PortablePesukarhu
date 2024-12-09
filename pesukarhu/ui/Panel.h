#pragma once

#include "pesukarhu/core/Scene.h"
#include "pesukarhu/core/input/InputEvent.h"

#include "UIFactories.h"
#include "GUIButton.h"
#include "InputField.h"
#include "Checkbox.h"

#include "pesukarhu/resources/Font.h"


#define UI_BASE_COLOR_COUNT 4

namespace pk
{
    namespace ui
    {
        class Panel
        {
        public:
            enum LayoutFillType
            {
                VERTICAL = 0,
                HORIZONTAL = 1
            };

            class PanelCursorPosEvent : public CursorPosEvent
            {
            private:
                Scene* _pScene = nullptr;
                Panel* _pPanel = nullptr;

            public:
                PanelCursorPosEvent(Scene* pScene, Panel* pPanel) :
                    _pScene(pScene),
                    _pPanel(pPanel)
                {}
        		virtual void func(int x, int y);
            };

        protected:
            friend class PanelCursorPosEvent;

            Scene* _pScene = nullptr;
            Font* _pDefaultFont = nullptr;
            entityID_t _entity;

            ConstraintProperties _constraintProperties;

            LayoutFillType _layoutType; // how added buttons, etc. are placed to panel
            vec2 _scale;

            float _slotPadding = 1.0f;
            vec2 _slotScale;
            vec3 _color;
            // Padding to add from where elements get added
            vec2 _offsetFromPanel = { 4.0f, 4.0f };

            int _slotCount = 0;

            bool _isMouseOver = false;

            static std::vector<vec4> s_uiColor;

            static int s_pickedPanels;

        public:
            Panel() {};
            virtual ~Panel() {};

            void create(
                Scene* pScene,
                Font* pDefaultFont,
                ConstraintProperties constraintProperties,
                vec2 scale,
                LayoutFillType fillType,
                vec3 color,
                GUIFilterType filter = GUIFilterType::GUI_FILTER_TYPE_NONE,
                float slotPadding = 1.0f,
                vec2 slotScale = vec2(200.0f, 24.0f)
            );

            void createDefault(
                Scene* pScene,
                Font* pDefaultFont,
                ConstraintProperties constraintProperties,
                vec2 scale,
                vec2 slotScale,
                LayoutFillType fillType = LayoutFillType::VERTICAL,
                int useColorIndex = 1
            );

            entityID_t addText(
                std::string txt,
                ConstraintProperties constraintProperties
            );
            entityID_t addText(std::string txt, vec3 color);
            entityID_t addDefaultText(std::string txt);

            GUIButton addDefaultButton(
                std::string txt,
                GUIButton::OnClickEvent* onClick,
                float width
            );

            GUIButton addButton(
                std::string txt,
                GUIButton::OnClickEvent* onClick,
                ConstraintProperties constraintProperties,
                vec2 scale
            );

            InputField addDefaultInputField(
                std::string infoTxt,
                int width,
                InputField::OnSubmitEvent* onSubmitEvent,
                bool clearOnSubmit = false,
                bool password = false
            );
            InputField addInputField(
                std::string infoTxt,
                ConstraintProperties constraintProperties,
                int width,
                InputField::OnSubmitEvent* onSubmitEvent,
                bool clearOnSubmit = false,
                bool password = false
            );

            entityID_t addImage(
                ConstraintProperties constraintProperties,
                float width, float height,
                Texture* pTexture,
                vec3 color,
                vec4 textureCropping = vec4(0, 0, 1, 1),
                GUIFilterType filter = GUIFilterType::GUI_FILTER_TYPE_NONE
            );

            pk::ui::Checkbox addDefaultCheckbox(std::string infoTxt);

            void setActive(bool arg, entityID_t entity = 0);

            void setLayer(int layer);

            // Returns current up to date pos and scale of the panel
            void getRect(float& outX, float& outY, float& outWidth, float& outHeight) const; // :D

            static vec4 get_base_ui_color(unsigned int colorIndex);
            // Values needs to be in range 0 - 255
            static void set_base_ui_color(unsigned int colorIndex, int r, int g, int b, int a);

            static bool is_mouse_over_ui();

            inline bool isMouseOver() const { return _isMouseOver; }
            inline entityID_t getEntity() const { return _entity; }

        private:
            vec2 calcNewSlotPos();
        };
    }
}
