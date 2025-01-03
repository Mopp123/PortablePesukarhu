#pragma once

#include "pesukarhu/Common.h"
#include "pesukarhu/graphics/Swapchain.h"
#include <string>


namespace pk
{
    class Application;

    class Window
    {
    protected:
        friend class Application;

        int _width = 800;
        int _height = 600;
        bool _fullscreen = false;

        Swapchain* _pSwapchain = nullptr;

    public:
        Window(int width,int height);
        Window(const Window&) = delete;
        virtual ~Window();
        virtual bool isCloseRequested() const { return false; }
        virtual void resize(int w, int h) = 0;

        static Window* create(
            PlatformName platform,
            GraphicsAPI graphicsAPI,
            const std::string& title,
            int width,
            int height,
            int MSAASamples = 2,
            bool fullscreen = false
        );

        inline int getWidth() const { return _width; }
        inline int getHeight() const { return _height; }

        // TODO: Make size and surface size actually separate
        inline int getSurfaceWidth() const { return _width; }
        inline int getSurfaceHeight() const { return _height; }

        inline bool isMinimized() const { return _width == 0; }

        inline bool isFullscreen() const { return _fullscreen; }

        inline Swapchain * const getSwapchain() const { return _pSwapchain; }

    protected:
        void createSwapchain();
    };
}
