#pragma once

#include <string>

#include "Pesukarhu/resources/Font.h"
#include "Pesukarhu/resources/Texture.h"
#include "Pesukarhu/ecs/Entity.h"
#include "Pesukarhu/ecs/components/ui/ConstraintData.h"
#include "Pesukarhu/ecs/components/renderable/GUIRenderable.h"
#include "Pesukarhu/ecs/components/renderable/TextRenderable.h"
#include "Pesukarhu/ecs/components/ui/UIElemState.h"
#include "Pesukarhu/core/input/InputEvent.h"


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

        struct ImgCreationProperties
        {
            ConstraintProperties constraintProperties;
            float width;
            float height;
            vec3 color = { 1, 1, 1 };
            vec3 highlightColor = { 1, 1, 1 };
            bool useHighlightColor = false;
            GUIFilterType filter = GUIFilterType::GUI_FILTER_TYPE_NONE;
            Texture* pTexture = nullptr;
            vec4 textureCropping = vec4(0, 0, 1, 1);
        };


        entityID_t create_image(ImgCreationProperties creationProperties);


        std::pair<entityID_t, TextRenderable*> create_text(
            const std::string& str, const Font& font,
            ConstraintProperties constraintProperties,
            vec3 color = vec3(1.0f, 1.0f, 1.0f),
            bool bold = false
        );


        // first = button img entity, second = button text entity
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

        // return pair of InputField-entity and TextRenderable ptr of its' content
        UIFactoryInputField create_input_field(
            std::string infoTxt, const Font& font,
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
    }
}
