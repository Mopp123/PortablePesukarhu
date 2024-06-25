#include <GL/glew.h>

#include "MasterRenderer.h"
#include "../core/Debug.h"
#include "RenderCommand.h"
#include "CommandBuffer.h"
#include "core/Application.h"
#include "core/Window.h"


namespace pk
{
    MasterRenderer::MasterRenderer()
    {
        const Application* pApp = Application::get();
        if (!pApp)
        {
            Debug::log(
                "@MasterRenderer::MasterRenderer Application was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        const Window* pWindow = pApp->getWindow();
        if (!pWindow)
        {
            Debug::log(
                "@MasterRenderer::MasterRenderer Application Window was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        _pSwapchain = pWindow->getSwapchain();
        if (!_pSwapchain)
        {
            Debug::log(
                "@MasterRenderer::MasterRenderer Swapchain was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }
    }

    MasterRenderer::~MasterRenderer()
    {}

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
        RenderCommand* pRenderCommand = RenderCommand::get();
        // TODO: catch possible std::runtime_error?
        // TODO: "Vulkanize" below better

        // "begin frame"
        uint32_t swapchainImgIndex = 0;
        AcquireSwapchainImgResult swapchainImgResult = _pSwapchain->acquireNextImage(
            &swapchainImgIndex
        );
        if (swapchainImgResult == AcquireSwapchainImgResult::RESIZE_REQUIRED)
        {
            handleWindowResize();
        }
        else if (swapchainImgResult == AcquireSwapchainImgResult::SUCCESS)
        {
            pRenderCommand->beginRenderPass();

            // Update common uniform buffers here?...

            // NOTE: Not sure if I like these being raw ptrs here...
            std::vector<CommandBuffer*> secondaryCmdBufs;
            for (const auto& renderer : _renderers)
            {
                // TODO: Switch below line to commented out style
                renderer.second->render(cam);
                // if (CommandBuffer* secondaryBuf = renderer.recordCmdBuf())
                //     secondaryCmdBufs.push_back(secondaryBuf);
            }

            pRenderCommand->endRenderPass();
            // TODO: submit primary command buffer to swapchain
        }
    }

    void MasterRenderer::handleWindowResize()
    {
        const Window* pWindow = Application::get()->getWindow();
        // TESTING
        glViewport(0, 0, pWindow->getSurfaceWidth(), pWindow->getSurfaceHeight());

        //_pRenderCommand->resizeViewport(w, h);
        if (!pWindow->isMinimized())
        {
            _pSwapchain->update();

            for (std::map<ComponentType, Renderer*>::const_iterator rIt = _renderers.begin(); rIt != _renderers.end(); ++rIt)
            {
                (*rIt).second->handleWindowResize();
            }
        }
    }
}
