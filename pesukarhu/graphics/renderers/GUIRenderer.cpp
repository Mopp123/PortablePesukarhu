#include "GUIRenderer.h"
#include <string>
#include "pesukarhu/core/Application.h"
#include "pesukarhu/ecs/components/renderable/GUIRenderable.h"
#include <unordered_map>


namespace pk
{
    /* Instance vertex buffer components:
        Float2 : pos
        Float2 : scale
        Float4 : color
        Float  : filter type
        Float4 : texture cropping
    */
    static const size_t s_instanceBufferComponents = 3 + 2 + 4 + 1 + 4;
    static const size_t s_maxInstanceCount = 400; // per batch, not total max count!
    GUIRenderer::GUIRenderer() :
        _vertexBufferLayout({}, VertexInputRate::VERTEX_INPUT_RATE_INSTANCE),
        _instanceBufferLayout({}, VertexInputRate::VERTEX_INPUT_RATE_INSTANCE),
        _textureDescSetLayout({}),
        _batchContainer(10, (sizeof(float) * s_instanceBufferComponents) * s_maxInstanceCount)
    {

        _pVertexShader = Shader::create_from_file(
            Shader::get_shader_path("GUIShader.vert"),
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT
        );
        _pFragmentShader = Shader::create_from_file(
            Shader::get_shader_path("GUIShader.frag"),
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT
        );

        // Vertex buffer layouts
        _vertexBufferLayout = VertexBufferLayout(
            {
                { 0, ShaderDataType::Float2 },
                { 1, ShaderDataType::Float2 }
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        );
        _instanceBufferLayout = VertexBufferLayout(
            {
                { 2, ShaderDataType::Float3 }, // pos
                { 3, ShaderDataType::Float2 }, // scale
                { 4, ShaderDataType::Float4 }, // color
                { 5, ShaderDataType::Float },  // filter type
                { 6, ShaderDataType::Float4 }  // texture cropping
            },
            VertexInputRate::VERTEX_INPUT_RATE_INSTANCE
        );

        // Textures desc set layout
        DescriptorSetLayoutBinding textureDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 1 }}
        );
        _textureDescSetLayout = DescriptorSetLayout({ textureDescSetLayoutBinding });

        // Atm creating these only for quick testing here!!!
        // Static vertex buffer
        const size_t vertexCount = 16;
        float vbData[vertexCount] = {
            0,  0,  0, 1,
            0, -1,  0, 0,
            1, -1,  1, 0,
            1,  0,  1, 1
        };
        unsigned short indices[6] =
        {
            0, 1, 2,
            2, 3, 0
        };
        _pVertexBuffer = Buffer::create(
            vbData,
            sizeof(float),
            vertexCount,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT,
            BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STATIC
        );
        _pIndexBuffer = Buffer::create(
            indices,
            sizeof(unsigned short),
            6,
            BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT,
            BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STATIC
        );
    }

    GUIRenderer::~GUIRenderer()
    {
        freeDescriptorSets();
        delete _pVertexShader;
        delete _pFragmentShader;
        delete _pVertexBuffer;
        delete _pIndexBuffer;
    }

    void GUIRenderer::initPipeline()
    {
        std::vector<VertexBufferLayout> vbLayouts({ _vertexBufferLayout, _instanceBufferLayout });
        std::vector<DescriptorSetLayout> descSetLayouts({ _textureDescSetLayout });

        const Window* pWindow = Application::get()->getWindow();
        const Swapchain* pSwapchain = pWindow->getSwapchain();
        Extent2D viewportExtent = pSwapchain->getSurfaceExtent();
        _pPipeline->init(
            vbLayouts,
            descSetLayouts,
            _pVertexShader, _pFragmentShader,
            (float)viewportExtent.width, (float)viewportExtent.height,
            { 0, 0, (uint32_t)viewportExtent.width, (uint32_t)viewportExtent.height },
            CullMode::CULL_MODE_NONE,
            FrontFace::FRONT_FACE_COUNTER_CLOCKWISE,
            true, // depth test
            DepthCompareOperation::COMPARE_OP_LESS_OR_EQUAL,
            false,
            0, ShaderStageFlagBits::SHADER_STAGE_NONE
        );
    }

    void GUIRenderer::submit(
        const Component* const renderableComponent,
        const mat4& transformation,
        void* pCustomData,
        size_t customDataSize
    )
    {
        const GUIRenderable * const pGuiRenderable = (const GUIRenderable * const)renderableComponent;

        const Texture* pTexture = Application::get()->getResourceManager().getWhiteTexture();
        if (pGuiRenderable->pTexture)
            pTexture = pGuiRenderable->pTexture;
        PK_id batchIdentifier = pTexture->getResourceID();

        // Figure out z pos depending on layer and camera config
        // TODO: Some more clever way of dealing with this
        const float maxLayers = (const float)UIRenderableComponent::get_max_layers();
        Scene* pScene = Application::get()->accessCurrentScene();
        entityID_t camEntity = pScene->activeCamera;
        Camera* pCamera = (Camera*)pScene->getComponent(camEntity, ComponentType::PK_CAMERA);
        if (!pCamera)
        {
            Debug::log(
                "@GUIRenderer::submit "
                "No camera component found from active camera entity: " + std::to_string(camEntity),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }

        float posZ = (-maxLayers + (float)pGuiRenderable->getLayer()) * pCamera->getGUILayerMultiplier();
        //float posZ = -((maxLayers + 1 - (float)pGuiRenderable->getLayer()) * pCamera->getGUILayerMultiplier());

        vec3 pos(transformation[0 + 3 * 4], transformation[1 + 3 * 4], posZ);
        vec2 scale(transformation[0 + 0 * 4], transformation[1 + 1 * 4]);

        const vec4 color = vec4(pGuiRenderable->color, 1.0f);
        const vec4& textureCropping = pGuiRenderable->textureCropping;

        float renderableData[s_instanceBufferComponents] = {
            pos.x, pos.y, pos.z,
            scale.x, scale.y ,
            color.x, color.y, color.z, color.w,
            (float)pGuiRenderable->filter,
            textureCropping.x, textureCropping.y, textureCropping.z, textureCropping.w
        };

        // Testing..
        if (_batchContainer.addData(renderableData, sizeof(float) * s_instanceBufferComponents, batchIdentifier))
        {
            // TODO: optimize below?
            if (!_batchContainer.hasDescriptorSets(batchIdentifier))
            {
                _batchContainer.createDescriptorSets(
                    batchIdentifier,
                    &_textureDescSetLayout,
                    { pTexture }
                );
            }
        }
    }

    void GUIRenderer::render()
    {
        if (!_pPipeline)
        {
            Debug::log(
                "@GUIRenderer::render pipeline is nullptr",
                Debug::MessageType::PK_ERROR
            );
        }

        Application* pApp = Application::get();

        const Scene* pCurrentScene = pApp->getCurrentScene();
        ResourceManager& resourceManager = pApp->getResourceManager();

        entityID_t cameraID = pCurrentScene->activeCamera;
        Camera* pCamera = (Camera*)pCurrentScene->getComponent(cameraID, ComponentType::PK_CAMERA);
        if (!pCamera)
        {
            Debug::log(
                "@GUIRenderer::render "
                "Scene's active camera was nullptr!",
                Debug::MessageType::PK_ERROR
            );
            return;
        }
        CommandBuffer* pCurrentCmdBuf = _pCommandBuffers[RenderPassType::RENDER_PASS_DIFFUSE][0];
        RenderCommand* pRenderCmd = RenderCommand::get();

        pRenderCmd->beginCmdBuffer(pCurrentCmdBuf);

        pRenderCmd->bindPipeline(
            pCurrentCmdBuf,
            PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
            _pPipeline
        );

        // TODO: get viewport extent from swapchain instead of window
        // TODO: specify viewport in pipeline instead by explicit command!
        const Window * const pWindow = Application::get()->getWindow();
        pRenderCmd->setViewport(
            pCurrentCmdBuf,
            0, 0,
            pWindow->getSurfaceWidth(), pWindow->getSurfaceHeight(),
            0.0f, 1.0f
        );

        pRenderCmd->bindIndexBuffer(
            pCurrentCmdBuf,
            _pIndexBuffer,
            0,
            IndexType::INDEX_TYPE_UINT16
        );

        //const std::unordered_map<PK_id, Batch*>& batches = _batchContainer.getOccupiedBatches();
        //std::unordered_map<PK_id, Batch*>::const_iterator batchIt;
        for (Batch* pBatch : _batchContainer.getBatches())
        {
            if (!pBatch->isOccupied())
                continue;

            // Figure out which texture using which resource
            // Quite dumb, but will do for now..
            Texture* pBatchTexture = (Texture*)resourceManager.getResource(pBatch->getIdentifier());
            if (!pBatchTexture)
            {
                Debug::log(
                    "@GUIRenderer::render "
                    "Failed find batch's texture from resource manager with id: " + std::to_string(pBatch->getIdentifier()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                continue;
            }

            Buffer* pBatchInstanceBuffer = pBatch->getBuffer();
            std::vector<Buffer*> vertexBuffers = { _pVertexBuffer, pBatchInstanceBuffer };
            pRenderCmd->bindVertexBuffers(
                pCurrentCmdBuf,
                0, 2,
                vertexBuffers
            );

            pRenderCmd->pushConstants(
                pCurrentCmdBuf,
                ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(mat4),
                &pCamera->getProjMat2D(),
                { { 0, ShaderDataType::Mat4 } }
            );

            /*
            std::unordered_map<PK_id, BatchDescriptorSets>::const_iterator descSetIt =  _descriptorSets.find(pBatch->getIdentifier());
            if (descSetIt == _descriptorSets.end())
            {
                Debug::log(
                    "@GUIRenderer::render "
                    "Couldn't find descriptor sets for batchID: " + std::to_string(pBatch->getIdentifier()),
                    Debug::MessageType::PK_ERROR
                );
                continue;
            }
            std::vector<const DescriptorSet*> toBind =
            {
                descSetIt->second.pTextureDescriptorSet[0]
            };
            */

            // TODO:
            // switch below (0) to current swapchaing img index when dealing with actual swapchain stuff..
            std::vector<const DescriptorSet*> toBind =
            {
                _batchContainer.getDescriptorSet(pBatch->getIdentifier(), 0)
            };

            pRenderCmd->bindDescriptorSets(
                pCurrentCmdBuf,
                PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
                0,
                toBind
            );

            pRenderCmd->drawIndexed(pCurrentCmdBuf, 6, pBatch->getInstanceCount(), 0, 0, 0);
        }

        pRenderCmd->endCmdBuffer(pCurrentCmdBuf);
    }

    void GUIRenderer::flush()
    {
        _batchContainer.clear();
    }

    void GUIRenderer::freeDescriptorSets()
    {
        /*
        std::unordered_map<PK_id, BatchDescriptorSets>::iterator it;
        for (it = _descriptorSets.begin(); it != _descriptorSets.end(); ++it)
        {
            for (DescriptorSet* pDescriptorSet : it->second.pTextureDescriptorSet)
                delete pDescriptorSet;
            it->second.pTextureDescriptorSet.clear();
        }
        _descriptorSets.clear();
        */
        _batchContainer.freeDescriptorSets();
    }
}
