#pragma once

#include "Pesukarhu/graphics/Swapchain.h"


namespace pk
{
    namespace opengl
    {
        class OpenglSwapchain : public Swapchain
        {
        public:
            virtual AcquireSwapchainImgResult acquireNextImage(uint32_t* pImgIndex);
            virtual void update();

        protected:
            friend class Swapchain;

            OpenglSwapchain();

            virtual void init();
            virtual void deinit();
        };
    }
}
