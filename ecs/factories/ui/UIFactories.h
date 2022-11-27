#pragma once

#include <string>

#include "../../systems/ui/Constraints.h"
#include "../../components/renderable/TextRenderable.h"
#include "../../components/UIElemState.h"
#include "../../../core/input/InputEvent.h"


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


        uint32_t create_image(
            ConstraintType horizontalType, float horizontalVal, 
            ConstraintType verticalType, float verticalVal, 
            float width, float height,
            vec3 color
        );


        uint32_t create_text(
            const std::string& str, 
            ConstraintType horizontalType, float horizontalVal, 
            ConstraintType verticalType, float verticalVal, 
            bool bold = false
        );


        uint32_t create_button(
            std::string txt,
            ConstraintType horizontalType, float horizontalVal, 
            ConstraintType verticalType, float verticalVal, 
            float width, float height,
            OnClickEvent* onClick,
            bool selectable = false,
            int txtDisplacementX = 5,
            int txtDisplacementY = 5,
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
