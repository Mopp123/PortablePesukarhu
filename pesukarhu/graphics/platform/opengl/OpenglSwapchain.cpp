#include "OpenglSwapchain.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/Window.h"
#include "pesukarhu/core/platform/desktop/DesktopWindow.h"
#include "pesukarhu/core/Debug.h"
#include <GLFW/glfw3.h>


namespace pk
{
    namespace opengl
    {
        OpenglSwapchain::OpenglSwapchain(Window* pWindow)
        {
            #ifndef PK_BUILD_WEB
            // TODO: Make this configurable
            glfwSwapInterval(0);
            // TODO:
            // *Validate this casting
            // *Make this work more consistently here
            //  -> we query for application window in those other funcs as well so why
            //  the fuck not just storing the window ptr here...
            //      -> on the other hand it makes sense to require passing
            //      the window when creating swapchain so it's harder to fuck up things like
            //      you have before (crashes because expecting Application::get()->someThing
            //      to exist at certain times...)
            _pGLFWwindow = ((desktop::DesktopWindow*)pWindow)->getGLFWwindow();
            if (!_pGLFWwindow)
            {
                Debug::log(
                    "@OpenglSwapchain::OpenglSwapchain "
                    "Window's GLFWwindow handle was nullptr!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            #endif

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

        void OpenglSwapchain::swap(uint32_t* imgIndex)
        {
            #ifdef PK_BUILD_DESKTOP
                if (_pGLFWwindow)
                    glfwSwapBuffers(_pGLFWwindow);
            #endif
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
