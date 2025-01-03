#include "MasterRenderer.h"
#include "Environment.h"
#include "pesukarhu/core/Debug.h"
#include "RenderCommand.h"
#include "CommandBuffer.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/Window.h"

#include "renderers/GUIRenderer.h"
#include "renderers/FontRenderer.h"
#include "renderers/StaticRenderer.h"
#include "renderers/SkinnedRenderer.h"
#include "renderers/TerrainRenderer.h"


namespace pk
{
    MasterRenderer::MasterRenderer() :
        _commonDescriptorSetLayout({}),
        _environmentDescriptorSetLayout({}),
        _directionalLightDescriptorSetLayout({})
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

        // NOTE: Requires recreating all common
        // uniform buffers and descriptor sets in case
        // swapchain's img count changes for some reason!!
        //
        // ...WHICH IS NOT DONE HERE ATM!!!!
        // TODO: DO THAT!!!
        // ALSO!
        // currently we are having just a single common ubo buffer
        // TODO: need to have one for each swapchain image!!!
        DescriptorSetLayoutBinding commonDescriptorSetLayoutBinding(
            0,
            2,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
            {
                { 0, ShaderDataType::Mat4 },
                { 1, ShaderDataType::Mat4 },
                { 2, ShaderDataType::Float4 }
            }
        );
        _commonDescriptorSetLayout = DescriptorSetLayout({commonDescriptorSetLayoutBinding});
        CommonUniforms initialCommonUniforms;
        _pCommonUniformBuffer = Buffer::create(
            &initialCommonUniforms,
            sizeof(CommonUniforms),
            1,
            BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_DYNAMIC,
            true
        );
        _pCommonDescriptorSet = new DescriptorSet(
            _commonDescriptorSetLayout,
            1,
            { _pCommonUniformBuffer }
        );

        // Environment properties
        DescriptorSetLayoutBinding environmentDescriptorSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
            {
                { 3, ShaderDataType::Float4 }
            }
        );
        _environmentDescriptorSetLayout = DescriptorSetLayout({environmentDescriptorSetLayoutBinding});
        EnvironmentProperties initialEnvProperties;
        _pEnvironmentUniformBuffer = Buffer::create(
            &initialEnvProperties,
            sizeof(EnvironmentProperties),
            1,
            BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_DYNAMIC,
            true
        );
        _pEnvironmentDescriptorSet = new DescriptorSet(
            _environmentDescriptorSetLayout,
            1,
            { _pEnvironmentUniformBuffer }
        );

        // Directional light properties
        DescriptorSetLayoutBinding dirLightDescriptorSetLayoutBinding(
            0,
            2,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {
                { 4, ShaderDataType::Float4 },
                { 5, ShaderDataType::Float4 }
            }
        );
        _directionalLightDescriptorSetLayout = DescriptorSetLayout({dirLightDescriptorSetLayoutBinding});
        DirectionalLightProperties initialDirLightProperties;
        _pDirectionalLightUniformBuffer = Buffer::create(
            &initialDirLightProperties,
            sizeof(DirectionalLightProperties),
            1,
            BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_DYNAMIC,
            true
        );
        _pDirectionalLightDescriptorSet = new DescriptorSet(
            _directionalLightDescriptorSetLayout,
            1,
            { _pDirectionalLightUniformBuffer }
        );

        _renderers[ComponentType::PK_RENDERABLE_GUI] = (Renderer*)(new GUIRenderer);
        _renderers[ComponentType::PK_RENDERABLE_TEXT] = (Renderer*)(new FontRenderer);
        _renderers[ComponentType::PK_RENDERABLE_STATIC3D] = (Renderer*)(new StaticRenderer);
        _renderers[ComponentType::PK_RENDERABLE_SKINNED] = (Renderer*)(new SkinnedRenderer);
        _renderers[ComponentType::PK_RENDERABLE_TERRAIN] = (Renderer*)(new TerrainRenderer);
    }

    void MasterRenderer::init()
    {
        for (auto& renderer : _renderers)
        {
            Debug::log(
                "@MasterRenderer::init "
                "Initializing pipeline for renderer of component type: " + component_type_to_string(renderer.first)
            );
            renderer.second->initPipeline();
        }
    }

    MasterRenderer::~MasterRenderer()
    {
        if (_pCommonDescriptorSet)
            delete _pCommonDescriptorSet;
        if (_pCommonUniformBuffer)
            delete _pCommonUniformBuffer;
        if (_pDirectionalLightDescriptorSet)
            delete _pDirectionalLightDescriptorSet;
        if (_pDirectionalLightUniformBuffer)
            delete _pDirectionalLightUniformBuffer;
        if (_pEnvironmentDescriptorSet)
            delete _pEnvironmentDescriptorSet;
        if (_pEnvironmentUniformBuffer)
            delete _pEnvironmentUniformBuffer;
    }

    void MasterRenderer::render(
        const mat4& persProjMatrix,
        const mat4& viewMatrix,
        const vec3& cameraPos
    )
    {
        RenderCommand* pRenderCommand = RenderCommand::get();
        const Scene* pScene = (const Scene*)Application::get()->getCurrentScene();

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
            pRenderCommand->beginRenderPass(_clearColor);

            // Update common uniform buffers here?...
            vec4 camPos(cameraPos, 1.0f);
            CommonUniforms commonUniforms = {
                 persProjMatrix,
                 viewMatrix,
                 camPos
            };
            _pCommonUniformBuffer->update(&commonUniforms, sizeof(CommonUniforms));

            // Update scene's environment properties
            _pEnvironmentUniformBuffer->update(
                &pScene->environmentProperties,
                sizeof(EnvironmentProperties)
            );

            // Update directional light ubo if scene has one..
            if (pScene->directionalLight != NULL_ENTITY_ID)
            {
                const DirectionalLight* pDirectionalLight = (const DirectionalLight*)pScene->getComponent(
                    pScene->directionalLight,
                    ComponentType::PK_LIGHT_DIRECTIONAL
                );
                DirectionalLightProperties directionalLight = {
                    vec4(pDirectionalLight->direction, 0.0f),
                    vec4(pDirectionalLight->color, 1.0f)
                };
                _pDirectionalLightUniformBuffer->update(
                    &directionalLight,
                    sizeof(DirectionalLightProperties)
                );
            }

            // NOTE: Not sure if I like these being raw ptrs here...
            std::vector<CommandBuffer*> secondaryCmdBufs;
            for (const auto& renderer : _renderers)
            {
                // TODO: Switch below line to commented out style
                renderer.second->render();
                // if (CommandBuffer* secondaryBuf = renderer.recordCmdBuf())
                //     secondaryCmdBufs.push_back(secondaryBuf);
            }

            pRenderCommand->endRenderPass();
            // TODO: Vulkanize -> actually record cmd bufs + also have the primary cmd buf
            //  On Vulkan, this attempts to exec primary cmd buf and vkQueuePresent
            _pSwapchain->swap(&swapchainImgIndex);
        }
    }

    void MasterRenderer::flush()
    {
        for (const auto& renderer : _renderers)
            renderer.second->flush();
    }

    Renderer* const MasterRenderer::getRenderer(ComponentType renderableType)
    {
        if (_renderers.find(renderableType) != _renderers.end())
            return _renderers[renderableType];
        return nullptr;
    }

    void MasterRenderer::freeDescriptorSets()
    {
        for (const auto& renderer : _renderers)
            renderer.second->freeDescriptorSets();
    }

    void MasterRenderer::handleSceneSwitch()
    {
        for (const auto& renderer : _renderers)
        {
            renderer.second->freeDescriptorSets();
            renderer.second->onSceneSwitch();
        }
    }

    void MasterRenderer::handleWindowResize()
    {
        const Window* pWindow = Application::get()->getWindow();

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
