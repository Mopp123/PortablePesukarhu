#pragma once

#include "../../Context.h"
#include "graphics/shaders/Shader.h"

namespace pk
{
    namespace web
    {
        class WebContext : public Context
        {
        public:
            WebContext();
            ~WebContext();
        };
    }
}
