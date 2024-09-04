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
        //  -> 6 used as push constant transformation matrix

        // TerrainMaterial desc set layout
        // Blendmap channel textures
        std::vector<DescriptorSetLayoutBinding> descSetBindings;
        for (int i = 0; i < TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES; ++i)
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
            TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES, // first after channel texture bindings
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 7 + TERRAIN_MATERIAL_MAX_CHANNEL_TEXTURES }} // TODO: some way to find out "common desc sets count"
        );
        descSetBindings.push_back(blendmapDescSetBinding);

        _materialDescSetLayout = DescriptorSetLayout(
            descSetBindings
        );

    }

    TerrainRenderer::~TerrainRenderer()
    {
        freeDescriptorSets();
        deleteRenderDataBuffers();
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
            CullMode::CULL_MODE_NONE,
            FrontFace::FRONT_FACE_COUNTER_CLOCKWISE,
            true,
            DepthCompareOperation::COMPARE_OP_LESS,
            sizeof(mat4),
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT
        );
    }

    void TerrainRenderer::submit(
        const Component* const renderableComponent,
        const mat4& transformation
    )
    {
        const TerrainRenderable * const pRenderable = (const TerrainRenderable * const)renderableComponent;
        if (_toRender.find(pRenderable) == _toRender.end())
            _toRender[pRenderable] = createTerrainRenderData(pRenderable, transformation);
        // recreate descriptor sets if necessary..
        if (_toRender[pRenderable].materialDescriptorSet.empty())
            createRenderDataDescriptorSets(_toRender[pRenderable]);

        _submittedTerrains.insert(pRenderable);
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


        // Get "common descriptor sets" from master renderer
        MasterRenderer& masterRenderer = pApp->getMasterRenderer();
        const DescriptorSet* pCommonDescriptorSet = masterRenderer.getCommonDescriptorSet();
        const DescriptorSet* pEnvDescriptorSet = masterRenderer.getEnvironmentDescriptorSet();
        const DescriptorSet* pDirLightDescriptorSet = masterRenderer.getDirectionalLightDescriptorSet();

        std::unordered_map<const TerrainRenderable*, TerrainRenderData>::const_iterator rIt;
        for (rIt = _toRender.begin(); rIt != _toRender.end(); ++rIt)
        {
            const TerrainRenderData& renderData = rIt->second;
            const Buffer* pIndexBuffer = renderData.pIndexBuffer;
            Buffer* pVertexBuffer = renderData.pVertexBuffer[0]; // TODO: this should be current swapchain img index

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

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            Debug::log("___TEST___GL ERR: " + std::to_string(err));
    }

    void TerrainRenderer::flush()
    {
        // Figure out if need to erase from _toRender
        std::unordered_map<const TerrainRenderable*, TerrainRenderData>::iterator it;
        for (it = _toRender.begin(); it != _toRender.end(); ++it)
        {
            if (_submittedTerrains.find(it->first) == _submittedTerrains.end())
                _toRender.erase(it->first);
        }
        _submittedTerrains.clear();
    }

    void TerrainRenderer::freeDescriptorSets()
    {
        std::unordered_map<const TerrainRenderable*, TerrainRenderData>::iterator it;
        for (it = _toRender.begin(); it != _toRender.end(); ++it)
        {
            TerrainRenderData& renderData = it->second;
            for (DescriptorSet* pDescSet : renderData.materialDescriptorSet)
                delete pDescSet;
            renderData.materialDescriptorSet.clear();
        }
    }

    void TerrainRenderer::handleSceneSwitch()
    {
        deleteRenderDataBuffers();
    }

    TerrainRenderData TerrainRenderer::createTerrainRenderData(
        const TerrainRenderable* pRenderable,
        mat4 transformationMatrix
    )
    {
        TerrainRenderData renderData;
        renderData.transformationMatrix = transformationMatrix;

        ResourceManager& resManager = Application::get()->getResourceManager();

        TerrainMaterial* pMaterial = (TerrainMaterial*)resManager.getResource(pRenderable->terrainMaterialID);
        renderData.channelTextures = {
            pMaterial->getChannelTexture(0),
            pMaterial->getChannelTexture(1),
            pMaterial->getChannelTexture(2),
            pMaterial->getChannelTexture(3)
        };
        renderData.pBlendmapTexture = pMaterial->getBlendmapTexture();

        createRenderDataBuffers(pRenderable->heightmap, pRenderable->tileWidth, renderData);
        createRenderDataDescriptorSets(renderData);

        return renderData;
    }

    // NOTE: Vertex data is generated from 0,0,0 towards +z direction
    void TerrainRenderer::createRenderDataBuffers(
        const std::vector<float>& heightmap,
        float tileWidth,
        TerrainRenderData& target
    )
    {
        // expecting heightmap always to be square
        uint32_t verticesPerRow = sqrt(heightmap.size());

		    std::vector<vec3> vertexPositions;
		    std::vector<vec3> vertexNormals;
		    std::vector<vec2> vertexTexCoords;

		    uint32_t vertexCount = 0;

        // actual "world space width"
		    float totalWidth = tileWidth * verticesPerRow;

		    for (int x = 0; x < verticesPerRow; x++)
		    {
		        for (int z = 0; z < verticesPerRow; z++)
		    	  {
		    		    vec3 vertexPos(x * tileWidth, heightmap[x + z * verticesPerRow], z * tileWidth);
		    		    vertexPositions.push_back(vertexPos);

		    		    // Figure out normal (quite shit and unpercise way, but it looks fine for now..)
                // Calc normal
					      float left = 0;
					      float right = 0;
					      float down = 0;
					      float up = 0;

					      int heightmapX = x;
					      int heightmapY = z;

					      if (heightmapX - 1 >= 0)
					      	  left = heightmap[(heightmapX - 1) + heightmapY * verticesPerRow];

					      if (heightmapX + 1 < verticesPerRow)
					      	  right = heightmap[(heightmapX + 1) + heightmapY * verticesPerRow];

					      if (heightmapY + 1 < verticesPerRow)
					      	  up = heightmap[heightmapX + (heightmapY + 1) * verticesPerRow];

					      if (heightmapY - 1 >= 0)
					          down = heightmap[heightmapX + (heightmapY - 1) * verticesPerRow];


                float heightVal = heightmap[x + z * verticesPerRow];
					      //vec3 normal((left - right), _heightModifier * 0.1f, (down - up)); // this is fucking dumb...
					      vec3 normal((left - right), 1.0f, (down - up)); // this is fucking dumb...
					      normal = normal.normalize();
					      vertexNormals.push_back(normal);

                // uv
		    		    vec2 texCoord(vertexPos.x / totalWidth, vertexPos.z / totalWidth);
		    		    vertexTexCoords.push_back(texCoord);

		    		    vertexCount++;
		    	  }
		    }

		    // Combine all into 1 buffer
		    std::vector<float> vertexData;
		    vertexData.reserve((3 + 3 + 2) * vertexCount);
		    for (int i = 0; i < vertexCount; i++)
		    {
            vertexData.emplace_back(vertexPositions[i].x);
            vertexData.emplace_back(vertexPositions[i].y);
            vertexData.emplace_back(vertexPositions[i].z);

            vertexData.emplace_back(vertexNormals[i].x);
            vertexData.emplace_back(vertexNormals[i].y);
            vertexData.emplace_back(vertexNormals[i].z);

            vertexData.emplace_back(vertexTexCoords[i].x);
            vertexData.emplace_back(vertexTexCoords[i].y);
		    }

		    std::vector<uint32_t> indices;
		    for (uint32_t x = 0; x < verticesPerRow; x++)
		    {
		    	  for (uint32_t z = 0; z < verticesPerRow; z++)
		    	  {
		    		    if (x >= verticesPerRow - 1 || z >= verticesPerRow - 1)
		    		        continue;

		    		    uint32_t topLeft = x + (z + 1) * verticesPerRow;
		    		    uint32_t bottomLeft = x + z * verticesPerRow;

		    		    uint32_t topRight = (x + 1) + (z + 1) * verticesPerRow;
		    		    uint32_t bottomRight = (x + 1) + z * verticesPerRow;


		    		    indices.push_back(bottomLeft);
		    		    indices.push_back(topLeft);
		    		    indices.push_back(topRight);

		    		    indices.push_back(topRight);
		    		    indices.push_back(bottomRight);
		    		    indices.push_back(bottomLeft);
		    	  }
		    }

        // Buffer for each swapchain img since we expect that the vertex buffer
        // may change due to altered heightmap!

        const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
        uint32_t swapchainImages = pSwapchain->getImageCount();
        std::vector<Buffer*> vertexBuffers(swapchainImages);
        for (int i = 0; i < swapchainImages; ++i)
        {
            vertexBuffers[i] = Buffer::create(
                vertexData.data(),
                sizeof(float),
                vertexData.size(),
                BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT,
                true // If we want to alter heights at runtime
            );
        }
        Buffer* pIndexBuffer = Buffer::create(
            indices.data(),
            sizeof(uint32_t),
            indices.size(),
            BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT
        );

        target.pVertexBuffer = vertexBuffers;
        target.pIndexBuffer = pIndexBuffer;
    }

    void TerrainRenderer::createRenderDataDescriptorSets(TerrainRenderData& target)
    {
        const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
        uint32_t swapchainImages = pSwapchain->getImageCount();
        target.materialDescriptorSet.resize(swapchainImages);
        for (int i = 0; i < swapchainImages; ++i)
        {
            target.materialDescriptorSet[i] = new DescriptorSet(
                _materialDescSetLayout,
                1,
                {
                    target.channelTextures[0],
                    target.channelTextures[1],
                    target.channelTextures[2],
                    target.channelTextures[3],
                    target.pBlendmapTexture
                }
            );
        }
    }

    void TerrainRenderer::deleteRenderDataBuffers()
    {
        std::unordered_map<const TerrainRenderable*, TerrainRenderData>::iterator it;
        for (it = _toRender.begin(); it != _toRender.end(); ++it)
        {
            for (Buffer* pBuf : it->second.pVertexBuffer)
                delete pBuf;
            delete it->second.pIndexBuffer;
        }
    }
}
