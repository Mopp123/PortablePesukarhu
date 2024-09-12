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
        _materialDescSetLayout({}),
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

        // Material desc set layout
        // Diffuse texture
        DescriptorSetLayoutBinding diffTextureDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 6 }}
        );
        // Specular texture
        DescriptorSetLayoutBinding specTextureDescSetLayoutBinding(
            1,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 7 }}
        );
        // "properties"
        DescriptorSetLayoutBinding materialPropsDescSetLayoutBinding(
            2,
            1,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 8, ShaderDataType::Float4 }}
        );

        _materialDescSetLayout = DescriptorSetLayout(
            {
                diffTextureDescSetLayoutBinding,
                specTextureDescSetLayoutBinding,
                materialPropsDescSetLayoutBinding
            }
        );

        // Create material "properties" ubo
        const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
        uint32_t swapchainImages = pSwapchain->getImageCount();
        vec4 initialMaterialProps(1.0f, 32.0f, 0.0f, 0.0f);
        for (int i = 0; i < swapchainImages; ++i)
        {
            Buffer* pMatUbo = Buffer::create(
                &initialMaterialProps,
                sizeof(vec4),
                1,
                BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                true
            );
            const Buffer* pConstCopy = (const Buffer*)pMatUbo;
            _materialPropsUniformBuffers.push_back(pMatUbo);
            _constMaterialPropsUniformBuffers.push_back(pConstCopy);
        }
    }

    StaticRenderer::~StaticRenderer()
    {
        freeDescriptorSets();
        delete _pVertexShader;
        delete _pFragmentShader;
    }

    void StaticRenderer::initPipeline()
    {
        const MasterRenderer& masterRenderer = Application::get()->getMasterRenderer();
        std::vector<VertexBufferLayout> vbLayouts({ _vertexBufferLayout, _instanceBufferLayout });
        std::vector<DescriptorSetLayout> descSetLayouts(
            {
                masterRenderer.getCommonDescriptorSetLayout(),
                masterRenderer.getEnvironmentDescriptorSetLayout(),
                masterRenderer.getDirectionalLightDescriptorSetLayout(),
                _materialDescSetLayout
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
            false,
            0, 0
        );
    }

    void StaticRenderer::submit(
        const Component* const renderableComponent,
        const mat4& transformation
    )
    {
        const Application* pApp = Application::get();
        const ResourceManager& resManager = pApp->getResourceManager();

        const Static3DRenderable * const pStaticRenderable = (const Static3DRenderable * const)renderableComponent;
        PK_id batchIdentifier = pStaticRenderable->meshID;
        const Mesh* pMesh = (const Mesh*)resManager.getResource(pStaticRenderable->meshID);
        if (!pMesh)
        {
            Debug::log(
                "@StaticRenderer::submit "
                "Renderable had invalid mesh",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        const Material* pMaterial = pMesh->getMaterial();
        const Texture_new* pDiffuseTexture = pMaterial->getDiffuseTexture(0);
        const Texture_new* pSpecularTexture = pMaterial->getSpecularTexture();

        _batchMaterialProperties[batchIdentifier] = {
            pMaterial->getSpecularStrength(),
            pMaterial->getSpecularShininess(),
            0.0f,
            0.0f
        };

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
                    &_materialDescSetLayout,
                    {
                        pDiffuseTexture,
                        pSpecularTexture
                    },
                    _constMaterialPropsUniformBuffers
                );
                //_batchMeshCache[batchIdentifier] = const_cast<Mesh*>(pMesh);
            }
        }
    }

    void StaticRenderer::render()
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

        ResourceManager& resManager = pApp->getResourceManager();
        for (Batch* pBatch : _batchContainer.getBatches())
        {
            if (!pBatch->isOccupied())
                continue;

            Mesh* pMesh = (Mesh*)resManager.getResource(pBatch->getIdentifier());
            if (!pMesh)
            {
                Debug::log(
                    "@StaticRenderer::render "
                    "Failed to find batch's mesh from resource manager with "
                    "batch identifier: " + std::to_string(pBatch->getIdentifier()),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                continue;
            }
            /*
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
            */

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
            MasterRenderer& masterRenderer = pApp->getMasterRenderer();
            // Update material properties ubo
            const vec4& materialProperties = _batchMaterialProperties[pBatch->getIdentifier()];
            _materialPropsUniformBuffers[0]->update(&materialProperties, sizeof(vec4));

            std::vector<const DescriptorSet*> toBind =
            {
                masterRenderer.getCommonDescriptorSet(),
                masterRenderer.getEnvironmentDescriptorSet(),
                masterRenderer.getDirectionalLightDescriptorSet(),
                _batchContainer.getDescriptorSet(pBatch->getIdentifier(), 0)
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
        //_batchMeshCache.clear();
    }
}
