#include "Renderer.h"
#include "core/Application.h"
#include "core/Window.h"


namespace pk
{
    Renderer::Renderer()
    {
        _pPipeline = Pipeline::create();

        const Application* pApp = Application::get();
        if (!pApp)
        {
            Debug::log(
                "@Renderer::Renderer Application was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        const Window* pWindow = pApp->getWindow();
        if (!pWindow)
        {
            Debug::log(
                "@Renderer::Renderer Application Window was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        const Swapchain* pSwapchain = pWindow->getSwapchain();
        if (!pSwapchain)
        {
            Debug::log(
                "@Renderer::Renderer Swapchain was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        // Create command buffers
        // These will persist through Renderer's lifetimr
        // NOTE: These will be "freed" and "re allocated" if
        // resizing window but the actual CommandBuffer objects
        // will remain. Only their internal content will be changed!
        uint32_t swapchainImages = pSwapchain->getImageCount();
        _pCommandBuffers[RenderPassType::RENDER_PASS_DIFFUSE] = CommandBuffer::create(swapchainImages);
    }

    Renderer::~Renderer()
    {
        delete _pPipeline;
        std::unordered_map<RenderPassType, std::vector<CommandBuffer*>>::const_iterator bIt;
        for (bIt = _pCommandBuffers.begin(); bIt != _pCommandBuffers.end(); ++bIt)
        {
            for (CommandBuffer* pBuf : (*bIt).second)
                delete pBuf;
        }
    }

    void Renderer::handleWindowResize()
    {
        _pPipeline->cleanUp();
        std::unordered_map<RenderPassType, std::vector<CommandBuffer*>>::const_iterator bIt;
        for (bIt = _pCommandBuffers.begin(); bIt != _pCommandBuffers.end(); ++bIt)
        {
            for (CommandBuffer* pBuf : (*bIt).second)
            {
                pBuf->free();
                pBuf->allocate();
            }
        }
        recreateDescriptorSets();
        initPipeline();
    }
}
