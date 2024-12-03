#pragma once

#include "Pesukarhu/graphics/Swapchain.h"


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
        virtual ~Window();

        virtual void resize(int w, int h) {};

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
