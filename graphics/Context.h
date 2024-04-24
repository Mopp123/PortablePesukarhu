#pragma once

// NOTE: This could cause issues. May require that ugly backtracking: "../../"
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
        virtual ~Context();
        static Context* create(PK_byte graphicsAPIType);

        static PK_byte get_api_type();
    };
}
