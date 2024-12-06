#pragma once

#include <string>

#include "pesukarhu/resources/Font.h"
#include "pesukarhu/resources/Texture.h"
#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/ecs/components/renderable/GUIRenderable.h"
#include "pesukarhu/ecs/components/renderable/TextRenderable.h"
#include "pesukarhu/ecs/components/ui/UIElemState.h"
#include "pesukarhu/core/input/InputEvent.h"


namespace pk
{
    namespace ui
    {
        // *User defined ui events
        class OnClickEvent
        {
        public:
            virtual void onClick(InputMouseButtonName button) = 0;
            virtual ~OnClickEvent(){}
        };


        class InputFieldOnSubmitEvent
        {
        public:
            virtual void onSubmit(std::string inputFieldText) = 0;
            virtual ~InputFieldOnSubmitEvent() {}
        };

        struct UIFactoryButton
        {
            entityID_t rootEntity = 0;
            entityID_t imgEntity = 0;
            entityID_t txtEntity = 0;
        };

        struct UIFactoryInputField
        {
            entityID_t rootEntity = 0;
            entityID_t contentEntity = 0;
        };

        struct UIFactoryCheckbox
        {
            entityID_t rootEntity;
            entityID_t checkedStatusImgEntity;
        };

        entityID_t create_text(
            const std::string& str, const Font& font,
            ConstraintProperties constraintProperties,
            vec3 color = vec3(1.0f, 1.0f, 1.0f),
            bool bold = false
        );


        UIFactoryButton create_button(
            std::string txt, const Font& font,
            ConstraintProperties constraintProperties,
            float width, float height,
            OnClickEvent* onClick,
            bool selectable = false,
            vec3 color = { 0.1f, 0.1f, 0.1f },
            vec3 textColor = { 1, 1, 1 },
            vec3 textHighlightColor = { 1, 1, 1 },
            vec3 backgroundHighlightColor = { 0.2f, 0.2f, 0.2f },
            GUIFilterType filter = GUIFilterType::GUI_FILTER_TYPE_NONE,
            Texture* pTexture = nullptr,
            vec4 textureCropping = vec4(0, 0, 1, 1)
        );

        UIFactoryInputField create_input_field(
            std::string infoTxt,
            const Font& font,
            ConstraintProperties constraintProperties,
            int width,
            InputFieldOnSubmitEvent* onSubmitEvent,
            bool clearOnSubmit = false,
            vec3 color = { 0.1f, 0.1f, 0.1f },
            vec3 textColor = { 1, 1, 1 },
            vec3 textHighlightColor = { 1, 1, 1 },
            vec3 backgroundHighlightColor = { 0.2f, 0.2f, 0.2f },
            bool password = false
        );

        UIFactoryCheckbox create_checkbox(
            std::string infoTxt,
            const Font* pFont,
            ConstraintProperties constraintProperties,
            vec3 backgroundColor = { 0.2f, 0.2f, 0.2f },
            vec3 backgroundHighlightColor = { 0.4f, 0.4f, 0.4f },
            vec3 checkedColor = { 0.4f, 0.4f, 0.4f }, // The color of the thing indicating checked status
            vec3 textColor = { 1, 1, 1 }
        );
    }
}
