#include "TerrainRenderer.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/ecs/components/renderable/TerrainRenderable.h"
#include "pesukarhu/resources/TerrainMesh.h"


namespace pk
{
    TerrainRenderer::TerrainRenderer() :
        _vertexBufferLayout(
            {
                { 0, ShaderDataType::Float3 }, // position
                { 1, ShaderDataType::Float3 }, // normal
                { 2, ShaderDataType::Float2 } // uv
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        ),
        _materialDescSetLayout({})
    {
        _pVertexShader = Shader::create_from_file(
            Shader::get_shader_path("TerrainShader.vert"),
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT
        );
        _pFragmentShader = Shader::create_from_file(
            Shader::get_shader_path("TerrainShader.frag"),
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT
        );

        // NOTE: common descriptors uniform locations for 3d stuff from master renderer
        // go from 0 to 5 -> 6 first available
        //  -> 6 used as push constant transformation matrix

        // Material desc set layout
        // Blendmap channel textures
        std::vector<DescriptorSetLayoutBinding> descSetBindings;
        for (int i = 0; i < TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS; ++i)
        {
            descSetBindings.push_back(
                DescriptorSetLayoutBinding(
                    i,
                    1,
                    DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
                    {{ 7 + i }}
                )
            );
        }

        DescriptorSetLayoutBinding blendmapDescSetBinding(
            TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS, // first after channel texture bindings
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 7 + TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS }} // TODO: some way to find out "common desc sets count"
        );
        descSetBindings.push_back(blendmapDescSetBinding);

        // Atm also experimenting with "custom data buffer" used as texture
        DescriptorSetLayoutBinding customDataDescSetBinding(
            TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS + 1,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 7 + TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS + 1 }} // TODO: some way to find out "common desc sets count"
        );
        descSetBindings.push_back(customDataDescSetBinding);

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
            DepthCompareOperation::COMPARE_OP_LESS_OR_EQUAL,
            false,
            sizeof(mat4),
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT
        );

    }

    void TerrainRenderer::submit(
        const Component* const renderableComponent,
        const mat4& transformation,
        void* pCustomData,
        size_t customDataSize
    )
    {
        const TerrainRenderable * const pRenderable = (const TerrainRenderable * const)renderableComponent;

        if (_toRender.find(pRenderable->terrainMeshID) == _toRender.end())
            _toRender[pRenderable->terrainMeshID] = createTerrainRenderData(pRenderable, transformation);
        else
            _toRender[pRenderable->terrainMeshID].transformationMatrix = transformation;

        // recreate descriptor sets if necessary..
        if (_toRender[pRenderable->terrainMeshID].materialDescriptorSet.empty())
            createRenderDataDescriptorSets(pRenderable, _toRender[pRenderable->terrainMeshID]);

        _submittedTerrains.insert(pRenderable->terrainMeshID);
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

        // Get "common descriptor sets" from master renderer
        MasterRenderer& masterRenderer = pApp->getMasterRenderer();
        const DescriptorSet* pCommonDescriptorSet = masterRenderer.getCommonDescriptorSet();
        const DescriptorSet* pEnvDescriptorSet = masterRenderer.getEnvironmentDescriptorSet();
        const DescriptorSet* pDirLightDescriptorSet = masterRenderer.getDirectionalLightDescriptorSet();

        ResourceManager& resManager = pApp->getResourceManager();

        std::unordered_map<PK_id, TerrainRenderData>::const_iterator rIt;
        for (rIt = _toRender.begin(); rIt != _toRender.end(); ++rIt)
        {
            const TerrainRenderData& renderData = rIt->second;
            TerrainMesh* pTerrainMesh = (TerrainMesh*)resManager.getResource(rIt->first);

            const Buffer* pIndexBuffer = pTerrainMesh->getIndexBuffer();;
            Buffer* pVertexBuffer = pTerrainMesh->accessVertexBuffer();

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

            pRenderCmd->bindVertexBuffers(
                pCurrentCmdBuf,
                0, 1,
                { pVertexBuffer }
            );

            std::vector<const DescriptorSet*> toBind =
            {
                pCommonDescriptorSet,
                pEnvDescriptorSet,
                pDirLightDescriptorSet,
                renderData.materialDescriptorSet[0] // TODO: this should be current swapchain img index
            };

            pRenderCmd->pushConstants(
                pCurrentCmdBuf,
                ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(mat4),
                renderData.transformationMatrix.getRawArray(),
                {
                    { 6, ShaderDataType::Mat4 }
                }
            );

            pRenderCmd->bindDescriptorSets(
                pCurrentCmdBuf,
                PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
                0,
                toBind
            );

            pRenderCmd->drawIndexed(
                pCurrentCmdBuf,
                indexBufLen,
                1,
                0,
                0,
                0
            );
        }

        pRenderCmd->endCmdBuffer(pCurrentCmdBuf);
    }

    void TerrainRenderer::flush()
    {
        // Figure out if need to erase from _toRender
        std::unordered_map<PK_id, TerrainRenderData>::iterator it;
        for (it = _toRender.begin(); it != _toRender.end(); ++it)
        {
            if (_submittedTerrains.find(it->first) == _submittedTerrains.end())
                _toRender.erase(it->first);
        }
        _submittedTerrains.clear();
    }

    void TerrainRenderer::freeDescriptorSets()
    {
        std::unordered_map<PK_id, TerrainRenderData>::iterator it;
        for (it = _toRender.begin(); it != _toRender.end(); ++it)
        {
            TerrainRenderData& renderData = it->second;
            for (DescriptorSet* pDescSet : renderData.materialDescriptorSet)
                delete pDescSet;
            renderData.materialDescriptorSet.clear();
        }
    }

    void TerrainRenderer::onSceneSwitch()
    {
        _toRender.clear();
    }

    TerrainRenderData TerrainRenderer::createTerrainRenderData(
        const TerrainRenderable* pRenderable,
        mat4 transformationMatrix
    )
    {
        TerrainRenderData renderData;
        renderData.transformationMatrix = transformationMatrix;
        createRenderDataDescriptorSets(pRenderable, renderData);

        return renderData;
    }

    void TerrainRenderer::createRenderDataDescriptorSets(
        const TerrainRenderable * const pRenderable,
        TerrainRenderData& target
    )
    {
        Application* pApp = Application::get();
        ResourceManager& resManager = pApp->getResourceManager();
        const TerrainMaterial* pMaterial = (const TerrainMaterial*)resManager.getResource(
            pRenderable->terrainMaterialID
        );
        std::vector<const Texture*> textures(TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS + 1 + 1); // first +1 is the blendmap texture, second +1 is "custom data texture"
        for (int i = 0; i < TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS; ++i)
            textures[i] = pMaterial->getChannelTexture(i);

        textures[TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS] = pMaterial->getBlendmapTexture();
        textures[TERRAIN_MATERIAL_MAX_TEXTURE_CHANNELS + 1] = pMaterial->getCustomDataTexture();

        const Swapchain* pSwapchain = pApp->getWindow()->getSwapchain();
        uint32_t swapchainImages = pSwapchain->getImageCount();
        target.materialDescriptorSet.resize(swapchainImages);
        for (int i = 0; i < swapchainImages; ++i)
        {
            target.materialDescriptorSet[i] = new DescriptorSet(
                _materialDescSetLayout,
                1,
                textures
            );
        }
    }
}
