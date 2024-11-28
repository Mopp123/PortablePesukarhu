#include "Window.h"
#include "core/Debug.h"


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
