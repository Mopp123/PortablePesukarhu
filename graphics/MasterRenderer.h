#pragma once

#include <vector>
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
        std::vector<Renderer*> _renderers;

    public:
        MasterRenderer(const std::vector<Renderer*>& renderers);
        ~MasterRenderer();

        void render(const Camera& cam);

        // To resize screen
        void handleWindowResize(int w, int h);
    };
}
