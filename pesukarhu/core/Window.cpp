#include "Window.h"
#include "platform/web/WebWindow.h"
#include "platform/desktop/DesktopWindow.h"
#include "Debug.h"


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
        switch (platform)
        {
            case PK_PLATFORM_WEB:
                return new web::WebWindow(width, height);
            case PK_PLATFORM_LINUX:
                return new desktop::DesktopWindow(
                    graphicsAPI,
                    title,
                    width,
                    height,
                    MSAASamples,
                    fullscreen
                );
            default:
                Debug::log(
                    "Failed to create window. Invalid platform: " + std::to_string(platform),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }

    void Window::createSwapchain()
    {
        _pSwapchain = Swapchain::create();
        if (!_pSwapchain)
            Debug::log(
                "@Window::createSwapchain Failed to assign window swapchain",
                Debug::MessageType::PK_FATAL_ERROR
            );
    }
}
