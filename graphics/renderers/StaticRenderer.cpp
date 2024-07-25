#include "StaticRenderer.h"
#include <string>
#include "core/Application.h"
#include "ecs/components/renderable/Static3DRenderable.h"

#include <GL/glew.h>


namespace pk
{
    static const size_t s_instanceBufferComponents = 16;
    static const size_t s_maxBatchInstances = 10000;
    static const size_t s_maxBatches = 10;
    StaticRenderer::StaticRenderer() :
        _vertexBufferLayout(
            {
                { 0, ShaderDataType::Float3 },
                { 1, ShaderDataType::Float3 },
                { 2, ShaderDataType::Float2 }
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        ),
        _instanceBufferLayout(
            {
                { 3, ShaderDataType::Mat4 }
            },
            VertexInputRate::VERTEX_INPUT_RATE_INSTANCE
        ),
        _textureDescSetLayout({}),
        _batchContainer(s_maxBatches, sizeof(float) * s_instanceBufferComponents * s_maxBatchInstances)
    {

        _pVertexShader = Shader::create_from_file(
            "assets/shaders/StaticShader.vert",
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT
        );
        _pFragmentShader = Shader::create_from_file(
            "assets/shaders/StaticShader.frag",
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT
        );

        // Textures desc set layout
        DescriptorSetLayoutBinding textureDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 5 }}
        );
        _textureDescSetLayout = DescriptorSetLayout({ textureDescSetLayoutBinding });

        initPipeline();
    }

    StaticRenderer::~StaticRenderer()
    {
        freeDescriptorSets();
        delete _pVertexShader;
        delete _pFragmentShader;
    }

    void StaticRenderer::initPipeline()
    {
        const MasterRenderer* pMasterRenderer = Application::get()->getMasterRenderer();
        std::vector<VertexBufferLayout> vbLayouts({ _vertexBufferLayout, _instanceBufferLayout });
        std::vector<DescriptorSetLayout> descSetLayouts(
            {
                pMasterRenderer->getCommonDescriptorSetLayout(),
                pMasterRenderer->getEnvironmentDescriptorSetLayout(),
                pMasterRenderer->getDirectionalLightDescriptorSetLayout(),
                _textureDescSetLayout
            }
        );

        const Window* pWindow = Application::get()->getWindow();
        const Swapchain* pSwapchain = pWindow->getSwapchain();
        Extent2D viewportExtent = pSwapchain->getSurfaceExtent();
        _pPipeline->init(
            vbLayouts,
            descSetLayouts,
            _pVertexShader, _pFragmentShader,
            (float)viewportExtent.width, (float)viewportExtent.height,
            { 0, 0, (uint32_t)viewportExtent.width, (uint32_t)viewportExtent.height },
            CullMode::CULL_MODE_BACK,
            FrontFace::FRONT_FACE_COUNTER_CLOCKWISE,
            true,
            DepthCompareOperation::COMPARE_OP_LESS,
            0, 0
        );
    }

    void StaticRenderer::submit(
        const Component* const renderableComponent,
        const mat4& transformation
    )
    {
        Application* pApp = Application::get();
        ResourceManager& resManager = pApp->getResourceManager();

        const Static3DRenderable * const pStaticRenderable = (const Static3DRenderable * const)renderableComponent;
        PK_id batchIdentifier = pStaticRenderable->meshID;
        Mesh* pMesh = (Mesh*)resManager.getResource(pStaticRenderable->meshID);
        if (!pMesh)
        {
            Debug::log(
                "@StaticRenderer::submit "
                "Renderable had invalid mesh",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        Material* pMaterial = pMesh->accessMaterial();
        // Atm just testing using white texture for debugging normals!
        //Texture_new* pTestTexture = resManager.getWhiteTexture();
        Texture_new* pTestTexture = pMaterial->accessTexture(0);

        if (_batchContainer.addData(
                transformation.getRawArray(),
                sizeof(float) * s_instanceBufferComponents,
                batchIdentifier
                )
            )
        {
            if (!_batchContainer.hasDescriptorSets(batchIdentifier))
            {
                _batchContainer.createDescriptorSets(
                    batchIdentifier,
                    &_textureDescSetLayout,
                    pTestTexture
                );
                _batchMeshCache[batchIdentifier] = pMesh;
            }
        }
    }

    void StaticRenderer::render(const Camera& cam)
    {
        if (!_pPipeline)
        {
            Debug::log(
                "@StaticRenderer::render pipeline is nullptr",
                Debug::MessageType::PK_ERROR
            );
        }

        Application* pApp = Application::get();
        const Scene* pCurrentScene = pApp->getCurrentScene();

        entityID_t cameraID = pCurrentScene->activeCamera;
        Camera* pCamera = (Camera*)pCurrentScene->getComponent(cameraID, ComponentType::PK_CAMERA);
        if (!pCamera)
        {
            Debug::log(
                "@StaticRenderer::render "
                "Scene's active camera was nullptr!",
                Debug::MessageType::PK_ERROR
            );
            return;
        }
        CommandBuffer* pCurrentCmdBuf = _pCommandBuffers[RenderPassType::RENDER_PASS_DIFFUSE][0];
        RenderCommand* pRenderCmd = RenderCommand::get();

        pRenderCmd->beginCmdBuffer(pCurrentCmdBuf);

        // TODO: get viewport extent from swapchain instead of window
        const Window * const pWindow = Application::get()->getWindow();

        pRenderCmd->setViewport(
            pCurrentCmdBuf,
            0, 0,
            pWindow->getSurfaceWidth(), pWindow->getSurfaceHeight(),
            0.0f, 1.0f
        );

        pRenderCmd->bindPipeline(
            pCurrentCmdBuf,
            PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
            _pPipeline
        );

        //std::unordered_map<PK_id, Batch*>::iterator bIt;
        //std::unordered_map<PK_id, Batch*>& occupiedBatches = _batchContainer.accessOccupiedBatches();
        //for (bIt = occupiedBatches.begin(); bIt != occupiedBatches.end(); ++bIt)
        for (Batch* pBatch : _batchContainer.getBatches())
        {
            if (!pBatch->isOccupied())
                continue;

            if (_batchMeshCache.find(pBatch->getIdentifier()) == _batchMeshCache.end())
            {
                Debug::log(
                    "@StaticRenderer::render "
                    "Failed to find batch's mesh from _batchMeshCache with "
                    "batch identifier: " + std::to_string(pBatch->getIdentifier()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                continue;
            }
            Mesh* pMesh = _batchMeshCache[pBatch->getIdentifier()];
            const Buffer* pIndexBuffer = pMesh->getIndexBuffer();
            Buffer* pVertexBuffer = pMesh->accessVertexBuffer();

            size_t indexBufLen = pIndexBuffer->getDataLength();
            IndexType indexType = IndexType::INDEX_TYPE_NONE;
            if (pIndexBuffer->getDataElemSize() == 2)
                indexType = IndexType::INDEX_TYPE_UINT16;
            if (pIndexBuffer->getDataElemSize() == 4)
                indexType = IndexType::INDEX_TYPE_UINT32;

            pRenderCmd->bindIndexBuffer(
                pCurrentCmdBuf,
                pIndexBuffer,
                0,
                indexType
            );

            Buffer* pBatchInstanceBuffer = pBatch->getBuffer();
            std::vector<Buffer*> vertexBuffers = { pVertexBuffer, pBatchInstanceBuffer };
            pRenderCmd->bindVertexBuffers(
                pCurrentCmdBuf,
                0, 2,
                vertexBuffers
            );

            // Get "common descriptor set" from master renderer
            MasterRenderer* pMasterRenderer = pApp->getMasterRenderer();
            std::vector<const DescriptorSet*> toBind =
            {
                pMasterRenderer->getCommonDescriptorSet(),
                pMasterRenderer->getEnvironmentDescriptorSet(),
                pMasterRenderer->getDirectionalLightDescriptorSet(),
                _batchContainer.getTextureDescriptorSet(pBatch->getIdentifier(), 0)
            };

            pRenderCmd->bindDescriptorSets(
                pCurrentCmdBuf,
                PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
                0,
                toBind
            );

            pRenderCmd->drawIndexed(
                pCurrentCmdBuf,
                indexBufLen,
                pBatch->getInstanceCount(),
                0,
                0,
                0
            );
        }

        pRenderCmd->endCmdBuffer(pCurrentCmdBuf);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            Debug::log("___TEST___GL ERR: " + std::to_string(err));
    }

    void StaticRenderer::flush()
    {
        _batchContainer.clear();
    }

    void StaticRenderer::freeDescriptorSets()
    {
        _batchContainer.freeDescriptorSets();
        _batchMeshCache.clear();
    }
}
