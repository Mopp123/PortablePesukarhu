#pragma once

#include "ecs/Entity.h"
#include <string>


namespace pk
{
    namespace ui
    {
        void set_input_field_content(
            const std::string& str,
            entityID_t inputFieldEntity,
            entityID_t txtRenderableEntity
        );
    }
}
