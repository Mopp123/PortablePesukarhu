#pragma once

#include <cstdint>

// if triple buffering: present(1) + in flight(2)
#define MAX_SWAPCHAIN_IMAGES 3


namespace pk
{
    class Window;

    enum AcquireSwapchainImgResult
    {
        SUCCESS,
        RESIZE_REQUIRED,
        ERROR
    };

    struct Extent2D
    {
        uint32_t width = 0;
        uint32_t height = 0;
    };


    class Swapchain
    {
    protected:
        friend class Window;

        uint32_t _imgCount = 0;
        Extent2D _surfaceExtent;

        bool _shouldResize = false;

    public:
        Swapchain(const Swapchain&) = delete;
        virtual ~Swapchain() {}

        virtual AcquireSwapchainImgResult acquireNextImage(uint32_t* pImgIndex) { return AcquireSwapchainImgResult::ERROR; }
        virtual void update() {}

        inline uint32_t getImageCount() const { return _imgCount; }
        inline Extent2D getSurfaceExtent() const { return _surfaceExtent; }

        inline void triggerResize() { _shouldResize = true; }
        inline bool shouldResize() const { return _shouldResize; }

    protected:
        // Allow creation to only happen through Window class
        static Swapchain* create();
        Swapchain() {}

        virtual void init() {}
        virtual void deinit() {}
    };
}
