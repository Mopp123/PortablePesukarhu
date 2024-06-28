#include <GL/glew.h>

#include "MasterRenderer.h"
#include "../core/Debug.h"
#include "RenderCommand.h"
#include "CommandBuffer.h"
#include "core/Application.h"
#include "core/Window.h"


namespace pk
{
    MasterRenderer::MasterRenderer() :
        _commonDescriptorSetLayout({})
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

        // NOTE: May require recreating all common
        // uniform buffers and descriptors in case
        // swapchain's img count changes for some reason!!
        DescriptorSetLayoutBinding commonDescriptorSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
            {
                { 0, ShaderDataType::Mat4 },
            }
        );
        _commonDescriptorSetLayout = DescriptorSetLayout({commonDescriptorSetLayoutBinding});
        CommonUniforms initialCommonUniforms;
        _pCommonUniformBuffer = Buffer::create(
            &initialCommonUniforms,
            sizeof(CommonUniforms),
            1,
            BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            true
        );
        _pCommonDescriptorSet = new DescriptorSet(
            _commonDescriptorSetLayout,
            1,
            {_pCommonUniformBuffer}
        );
    }

    MasterRenderer::~MasterRenderer()
    {
        if (_pCommonDescriptorSet)
            delete _pCommonDescriptorSet;
        if (_pCommonUniformBuffer)
            delete _pCommonUniformBuffer;
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
            Camera* pSceneCamera = Application::get()->getCurrentScene()->activeCamera;
            const mat4 projMat = pSceneCamera->getProjMat2D();

            CommonUniforms commonUniforms = { projMat };
            _pCommonUniformBuffer->update(&commonUniforms, sizeof(CommonUniforms));

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

    Renderer* const MasterRenderer::getRenderer(ComponentType renderableType)
    {
        if (_renderers.find(renderableType) != _renderers.end())
            return _renderers[renderableType];
        return nullptr;
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
