#pragma once

#include "pesukarhu/graphics/Context.h"
#include "pesukarhu/graphics/shaders/Shader.h"

namespace pk
{
    namespace web
    {
        class WebContext : public Context
        {
        private:
            int32_t _maxTextureUnits = 0;

        public:
            WebContext();
            ~WebContext();

            inline int32_t getMaxTextureUnits() const { return _maxTextureUnits; }
        };
    }
}
