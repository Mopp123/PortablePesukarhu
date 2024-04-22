#pragma once

#include "Common.h"

#define GRAPHICS_API_NONE 0
#define GRAPHICS_API_WEBGL 1


namespace pk
{
    class Context
    {
    protected:
        static PK_byte s_graphicsAPI;

    public:
        Context(PK_byte graphicsAPIType);
        virtual ~Context();
        static PK_byte get_api_type();
    };
}
