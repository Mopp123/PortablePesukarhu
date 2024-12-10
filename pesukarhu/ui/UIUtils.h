#pragma once

#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/resources/Font.h"
#include <string>


namespace pk
{
    namespace ui
    {
        float get_text_visual_width(const std::string& txt, const Font* pFont);
    }
}
