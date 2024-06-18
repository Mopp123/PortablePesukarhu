#include "MasterRenderer.h"
#include "../core/Debug.h"

namespace pk
{
    MasterRenderer::MasterRenderer()
    {
        _pRenderCommand = RenderCommand::create();
    }

    MasterRenderer::~MasterRenderer()
    {
        delete _pRenderCommand;
    }

    void MasterRenderer::addRenderer(ComponentType renderableComponentType, Renderer* renderer)
    {
        if (_renderers.find(renderableComponentType) != _renderers.end())
        {
            Debug::log(
                "Attempted to add Renderer to MasterRenderer but renderer of assigned renderable component type already exists",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }
        else
        {
            _renderers[renderableComponentType] = renderer;
        }
    }

    void MasterRenderer::render(const Camera& cam)
    {
        // TODO: catch possible std::runtime_error
        // TODO: "Vulkanize" below better
        _pRenderCommand->beginFrame();
        _pRenderCommand->beginRenderPass();

        // Update common uniform buffers here...

        // NOTE: Not sure if I like these being raw ptrs here...
        std::vector<CommandBuffer*> secondaryCmdBufs;
        for (const auto& renderer : _renderers)
        {
            // TODO: Switch below line to commented out style
            renderer.second->render(cam);
            // if (CommandBuffer* secondaryBuf = renderer.recordCmdBuf())
            //     secondaryCmdBufs.push_back(secondaryBuf);
        }

        _pRenderCommand->endRenderPass();
        _pRenderCommand->endFrame();
    }

    void MasterRenderer::handleWindowResize(int w, int h)
    {
        _pRenderCommand->resizeViewport(w, h);
    }
}
