#pragma once

#include "pesukarhu/graphics/Swapchain.h"

#ifdef PK_BUILD_DESKTOP
    #include <GLFW/glfw3.h>
#endif


namespace pk
{
    namespace opengl
    {
        class OpenglSwapchain : public Swapchain
        {
        private:
            #ifdef PK_BUILD_DESKTOP
                GLFWwindow* _pGLFWwindow = nullptr;
            #endif

        public:
            virtual AcquireSwapchainImgResult acquireNextImage(uint32_t* pImgIndex);
            virtual void swap(uint32_t* imgIndex);
            virtual void update();

        protected:
            friend class Swapchain;

            OpenglSwapchain(Window* pWindow);

            virtual void init();
            virtual void deinit();
        };
    }
}
