#pragma once

#include "pesukarhu/Common.h"
#include "pesukarhu/core/Window.h"


namespace pk
{
    class Context
    {
    protected:
        static GraphicsAPI s_graphicsAPI;

    public:
        virtual ~Context();
        static Context* create(PlatformName platform, GraphicsAPI graphicsAPI, Window* pWindow);

        static GraphicsAPI get_graphics_api();
    };
}
