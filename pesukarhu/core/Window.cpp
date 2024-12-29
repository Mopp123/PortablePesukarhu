#include "Window.h"
#include "Debug.h"

#ifdef PK_BUILD_WEB
    #include "platform/web/WebWindow.h"
#elif defined(PK_BUILD_DESKTOP)
    #include "platform/desktop/DesktopWindow.h"
#endif


namespace pk
{
    Window::Window(int width, int height) :
        _width(width), _height(height)
    {
    }

    Window::~Window()
    {
        if (_pSwapchain)
            delete _pSwapchain;
    }

    Window* Window::create(
        PlatformName platform,
        GraphicsAPI graphicsAPI,
        const std::string& title,
        int width,
        int height,
        int MSAASamples,
        bool fullscreen
    )
    {
        #ifdef PK_BUILD_WEB
            return new web::WebWindow(width, height);
        #elif defined(PK_BUILD_DESKTOP)
            return new desktop::DesktopWindow(
                graphicsAPI,
                title,
                width,
                height,
                MSAASamples,
                fullscreen
            );
        #else
            Debug::log(
                "@Window::create "
                "Failed to create Window. Invalid build target! "
                "Available targets: web, desktop(linux)",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        #endif
    }

    void Window::createSwapchain()
    {
        _pSwapchain = Swapchain::create(this);
        if (!_pSwapchain)
            Debug::log(
                "@Window::createSwapchain Failed to assign window swapchain",
                Debug::MessageType::PK_FATAL_ERROR
            );
    }
}
