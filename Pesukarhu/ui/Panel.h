#pragma once

#include "Pesukarhu/core/Scene.h"
#include "Pesukarhu/core/input/InputEvent.h"

#include "UIFactories.h"

#include "Pesukarhu/resources/Font.h"


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

            std::pair<entityID_t, TextRenderable*> addText(
                std::string txt,
                ConstraintProperties constraintProperties
            );
            std::pair<entityID_t, TextRenderable*> addText(std::string txt, vec3 color);
            std::pair<entityID_t, TextRenderable*> addDefaultText(std::string txt);

            UIFactoryButton addDefaultButton(
                std::string txt,
                OnClickEvent* onClick,
                float width
            );

            UIFactoryButton addButton(
                std::string txt,
                OnClickEvent* onClick,
                ConstraintProperties constraintProperties,
                vec2 scale
            );

            UIFactoryInputField addDefaultInputField(
                std::string infoTxt,
                int width,
                InputFieldOnSubmitEvent* onSubmitEvent,
                bool clearOnSubmit = false,
                bool password = false
            );
            UIFactoryInputField addInputField(
                std::string infoTxt,
                ConstraintProperties constraintProperties,
                int width,
                InputFieldOnSubmitEvent* onSubmitEvent,
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
