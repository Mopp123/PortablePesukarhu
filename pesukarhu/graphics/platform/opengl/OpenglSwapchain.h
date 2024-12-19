#pragma once

#include "pesukarhu/graphics/Swapchain.h"
#include <GLFW/glfw3.h>


namespace pk
{
    namespace opengl
    {
        class OpenglSwapchain : public Swapchain
        {
        private:
            GLFWwindow* _pGLFWwindow = nullptr;

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
