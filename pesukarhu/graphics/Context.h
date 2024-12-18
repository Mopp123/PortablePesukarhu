#pragma once

#include "pesukarhu/Common.h"


namespace pk
{
    enum GraphicsAPI
    {
        PK_GRAPHICS_API_NONE = 0,
        PK_GRAPHICS_API_WEBGL,
        PK_GRAPHICS_API_GL
    };


    class Context
    {
    protected:
        static GraphicsAPI s_graphicsAPI;

    public:
        virtual ~Context();
        static Context* create(GraphicsAPI graphicsAPI);

        static GraphicsAPI get_graphics_api();
    };
}
