#include "OpenglSwapchain.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/Window.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
    namespace opengl
    {
        OpenglSwapchain::OpenglSwapchain()
        {
            _imgCount = 1;
            init();
        }

        AcquireSwapchainImgResult OpenglSwapchain::acquireNextImage(uint32_t* pImgIndex)
        {
        #ifdef PK_DEBUG_FULL
            if (!pImgIndex)
            {
                Debug::log(
                    "@OpenglSwapchain::acquireNextImage inputted pImgIndex was nullptr",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return AcquireSwapchainImgResult::ERROR;
            }
        #endif
            if (_shouldResize)
                return AcquireSwapchainImgResult::RESIZE_REQUIRED;
            // Always return first one since here we are just "faking" swapchain..
            *pImgIndex = 0;
            return AcquireSwapchainImgResult::SUCCESS;
        }

        void OpenglSwapchain::init()
        {
            const Application* pApp = Application::get();
            if (!pApp)
            {
                Debug::log(
                    "@OpenglSwapchain::OpenglSwapchain Application was nullptr",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            const Window* pWindow = pApp->getWindow();
            if (!pWindow)
            {
                Debug::log(
                    "@OpenglSwapchain::OpenglSwapchain Application Window was nullptr",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            _surfaceExtent.width = pWindow->getSurfaceWidth();
            _surfaceExtent.height = pWindow->getSurfaceHeight();
        }

        void OpenglSwapchain::deinit()
        {
        }

        void OpenglSwapchain::update()
        {
            deinit();
            init();
            _shouldResize = false;
            Debug::log("Swapchain updated");
        }
    }
}
