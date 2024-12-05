#pragma once

#include "Pesukarhu/ecs/Entity.h"
#include <string>


namespace pk
{
    namespace ui
    {
        std::string get_input_field_content(entityID_t inputFieldEntity);
        void set_input_field_content(
            const std::string& str,
            entityID_t inputFieldEntity,
            entityID_t txtRenderableEntity
        );

        bool get_checkbox_status(entityID_t checkboxEntity);
        void set_checkbox_status(
            entityID_t checkboxEntity,
            entityID_t checkedStatusImgEntity,
            bool checked
        );
    }
}
