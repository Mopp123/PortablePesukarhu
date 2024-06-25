#pragma once

#include <map>
#include "Renderer.h"
#include "../ecs/components/Camera.h"
#include "Swapchain.h"


namespace pk
{
    class MasterRenderer
    {
    private:
        Swapchain* _pSwapchain = nullptr;
        std::map<ComponentType, Renderer*> _renderers;

    public:
        MasterRenderer();
        ~MasterRenderer();
        void addRenderer(ComponentType renderableComponentType, Renderer* renderer);

        void render(const Camera& cam);

        inline Renderer* const getRenderer(ComponentType renderableType) { return _renderers[renderableType]; }

    private:
        void handleWindowResize();
    };
}
