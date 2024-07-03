#pragma once

#include <string>

#include "ecs/components/ui/ConstraintData.h"
#include "../../components/renderable/TextRenderable.h"
#include "../../components/UIElemState.h"
#include "../../../core/input/InputEvent.h"

#include "graphics/Texture.h"


namespace pk
{
    namespace ui
    {
        // To track if clicked on multiple overlapping ui elems, etc.
        class EventOverlapResolver
        {
        public:


        };

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


        uint32_t create_image(
            ConstraintType horizontalType, float horizontalVal,
            ConstraintType verticalType, float verticalVal,
            float width, float height,
            bool drawBorder = false,
            Texture* texture = nullptr,
            vec4 textureCropping = vec4(0, 0, 1, 1),
            vec3 color = vec3(0, 0, 0),
            Texture_new* pTexture = nullptr
        );


        std::pair<uint32_t, TextRenderable*> create_text(
            const std::string& str,
            ConstraintType horizontalType, float horizontalVal,
            ConstraintType verticalType, float verticalVal,
            vec3 color = vec3(1.0f, 1.0f, 1.0f),
            bool bold = false
        );


        uint32_t create_button(
            std::string txt,
            ConstraintType horizontalType, float horizontalVal,
            ConstraintType verticalType, float verticalVal,
            float width, float height,
            OnClickEvent* onClick,
            bool selectable = false,
            Texture* texture = nullptr,
            vec4 textureCropping = vec4(0, 0, 1, 1),
            vec3 color = vec3(0.1f, 0.1f, 0.1f),
            int txtDisplacementX = 8,
            int txtDisplacementY = 4,
            UIElemState* pUIElemState = nullptr
        );

        // return pair of InputField-entity and TextRenderable ptr of its' content
        std::pair<uint32_t, TextRenderable*> create_input_field(
            std::string infoTxt,
            ConstraintType horizontalType, float horizontalVal,
            ConstraintType verticalType, float verticalVal,
            int width,
            InputFieldOnSubmitEvent* onSubmitEvent,
            bool clearOnSubmit = false
        );
    }
}
