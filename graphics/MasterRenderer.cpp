#include "MasterRenderer.h"

namespace pk
{
    MasterRenderer::MasterRenderer(const std::vector<Renderer*>& renderers) :
        _renderers(renderers) // NOTE: Not sure if this works? (assigning that vec to another?)
    {
        _pRenderCommand = RenderCommand::create();
    }

    MasterRenderer::~MasterRenderer()
    {
        delete _pRenderCommand;
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
        for (Renderer* renderer : _renderers)
        {
            if (CommandBuffer* secondaryBuf = renderer.recordCmdBuf())
                secondaryCmdBufs.push_back(secondaryBuf);
        }

        _pRenderCommand->endRenderPass();
        _pRenderCommand->endFrame();
    }

    void MasterRenderer::handleWindowResize(int w, int h)
    {
        _pRenderCommand->resizeViewport(w, h);
    }

    // void MasterRenderer::beginFrame()
    // {}

    // void MasterRenderer::beginRenderPass()
    // {}

    // void MasterRenderer::endRenderPass()
    // {}

    // void MasterRenderer::endFrame()
    // {}
}
