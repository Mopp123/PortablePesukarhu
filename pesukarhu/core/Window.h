#pragma once

#include "pesukarhu/Common.h"
#include "pesukarhu/graphics/Swapchain.h"


namespace pk
{
    class Application;

    class Window
    {
    protected:
        friend class Application;

        int _width = 800;
        int _height = 600;

        Swapchain* _pSwapchain = nullptr;

    public:
        Window(int width, int height);
        Window(const Window&) = delete;
        virtual ~Window();

        virtual void resize(int w, int h) {};

        static Window* create(PlatformName platform, int width, int height);

        inline int getWidth() const { return _width; }
        inline int getHeight() const { return _height; }

        // TODO: Make size and surface size actually separate
        inline int getSurfaceWidth() const { return _width; }
        inline int getSurfaceHeight() const { return _height; }

        inline bool isMinimized() const { return _width == 0; }

        inline Swapchain * const getSwapchain() const { return _pSwapchain; }

    protected:
        void createSwapchain();
    };
}
