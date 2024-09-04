#include "TerrainRenderer.h"
#include "core/Application.h"
#include "ecs/components/renderable/TerrainRenderable.h"

#include <GL/glew.h>


namespace pk
{
    TerrainRenderer::TerrainRenderer() :
        _vertexBufferLayout(
            {
                { 0, ShaderDataType::Float3 },
                { 1, ShaderDataType::Float3 },
                { 2, ShaderDataType::Float2 }
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        ),
        _materialDescSetLayout({})
    {
        _pVertexShader = Shader::create_from_file(
            "assets/shaders/TerrainShader.vert",
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT
        );
        _pFragmentShader = Shader::create_from_file(
            "assets/shaders/TerrainShader.frag",
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT
        );

        // NOTE: common descriptors uniform locations for 3d stuff from master renderer
        // go from 0 to 5 -> 6 first available

        // TerrainMaterial desc set layout
        // Blendmap channel textures
        std::vector<DescriptorSetLayoutBinding> descSetBindings(TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES);
        for (int i = 0; i < TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES; ++i)
        {
            descSetBindings[i] = DescriptorSetLayoutBinding(
                i,
                1,
                DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
                {{ 6 + i }}
            );
        }
        DescriptorSetLayoutBinding blendmapDescSetBinding(
            TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES, // first after channel texture bindings
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 6 + TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES }} // TODO: some way to find out "common desc sets count"
        );

        _materialDescSetLayout = DescriptorSetLayout(
            descSetBindings
        );
    }

    TerrainRenderer::~TerrainRenderer()
    {
        freeDescriptorSets();
        delete _pVertexShader;
        delete _pFragmentShader;
    }

    void TerrainRenderer::initPipeline()
    {
        const MasterRenderer& masterRenderer = Application::get()->getMasterRenderer();
        std::vector<VertexBufferLayout> vbLayouts({ _vertexBufferLayout });
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
            0, 0
        );
    }

    void TerrainRenderer::submit(
        const Component* const renderableComponent,
        const mat4& transformation
    )
    {
        const TerrainRenderable * const pRenderable = (const TerrainRenderable * const)renderableComponent;
        _toRender.push_back(pRenderable);
    }

    void TerrainRenderer::render()
    {
        if (!_pPipeline)
        {
            Debug::log(
                "@TerrainRenderer::render pipeline is nullptr",
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
                "@TerrainRenderer::render "
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


        ResourceManager& resManager = pApp->getResourceManager();
        for (const TerrainRenderable* pRenderable : _toRender)
        {
            // TODO: What if mesh changes between frames and triple buffering..?
            //  -> should it be concern of mesh or renderer?
            //      -> maybe some "dynamc mesh system" so each renderer wouldn't need
            //      to handle same issue in the same way?
            Mesh* pMesh = (Mesh*)resManager.getResource(pRenderable->meshID);
            if (!pMesh)
            {
                Debug::log(
                    "@TerrainRenderer::render "
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
                    "@TerrainRenderer::render "
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

    void TerrainRenderer::flush()
    {
        _batchContainer.clear();
    }

    void TerrainRenderer::freeDescriptorSets()
    {
        _batchContainer.freeDescriptorSets();
        //_batchMeshCache.clear();
    }
}
