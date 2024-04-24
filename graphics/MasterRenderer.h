#pragma once

#include <vector>
#include <map>
#include "Context.h"
#include "CommandBuffer.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "../ecs/components/Camera.h"


namespace pk
{
    class MasterRenderer
    {
    private:
        RenderCommand* _pRenderCommand = nullptr;
        std::map<ComponentType, Renderer*> _renderers;

    public:
        MasterRenderer();
        ~MasterRenderer();
        void addRenderer(ComponentType renderableComponentType, Renderer* renderer);

        void render(const Camera& cam);

        void handleWindowResize(int w, int h);

        inline Renderer* const getRenderer(ComponentType renderableType) { return _renderers[renderableType]; }
    };
}
