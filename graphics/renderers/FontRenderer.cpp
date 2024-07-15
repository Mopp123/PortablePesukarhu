#include "FontRenderer.h"
#include <string>
#include "core/Application.h"
#include "ecs/components/renderable/GUIRenderable.h"

// TODO: dont include this here, atm just for getting gl errors..
#include <GL/glew.h>


namespace pk
{

    // NOTE: Atm these here only for quick testing!
    static std::string s_vertexSource = R"(
        precision mediump float;

        attribute vec2 vertexPos;

        attribute vec2 pos;
        attribute vec2 scale;
        attribute vec2 texOffset;
        attribute vec4 color;

        struct PushConstantData
        {
            mat4 projMat;
            float atlasRows;
        };
        uniform PushConstantData pushConstants;

        varying vec2 var_uvCoord;
        varying vec4 var_color;

        void main()
        {
            vec2 vpos = vertexPos;
            gl_Position = pushConstants.projMat * vec4((vpos * scale) + pos, 0, 1.0);
            var_uvCoord = (vpos * vec2(1.0, -1.0) + texOffset) / pushConstants.atlasRows;
            var_color = color;
        }
    )";

    static std::string s_fragmentSource = R"(
        precision mediump float;

        varying vec2 var_uvCoord;
        varying vec4 var_color;

        uniform sampler2D texSampler;

        void main()
        {
	    vec4 texColor = texture2D(texSampler, var_uvCoord);
	    vec4 finalColor = var_color * texColor.a * 1.0; // The last * 1.0 is thickness..

	    if(texColor.a <= 0.0)
	        discard;

	    gl_FragColor = vec4(finalColor.rgb, 1.0);
        }
    )";

    static const size_t s_instanceBufferComponents = 2 * 3 + 4;
    static const size_t s_maxInstanceCount = 400; // per batch, not total max count!
    FontRenderer::FontRenderer() :
        _vertexBufferLayout({}, VertexInputRate::VERTEX_INPUT_RATE_INSTANCE),
        _instanceBufferLayout({}, VertexInputRate::VERTEX_INPUT_RATE_INSTANCE),
        _textureDescSetLayout({}),
        _batchContainer(10, (sizeof(float) * s_instanceBufferComponents) * s_maxInstanceCount)
    {
        _pVertexShader = Shader::create(s_vertexSource, ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT);
        _pFragmentShader = Shader::create(s_fragmentSource, ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT);

        // Vertex buffer layouts
        _vertexBufferLayout = VertexBufferLayout(
            {
                { 0, ShaderDataType::Float2 }
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        );
        _instanceBufferLayout = VertexBufferLayout(
            {
                { 1, ShaderDataType::Float2 }, // pos
                { 2, ShaderDataType::Float2 }, // scale
                { 3, ShaderDataType::Float2 }, // texture offset
                { 4, ShaderDataType::Float4 } // color
            },
            VertexInputRate::VERTEX_INPUT_RATE_INSTANCE
        );

        // Textures desc set layout
        DescriptorSetLayoutBinding textureDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 2 }}
        );
        _textureDescSetLayout = DescriptorSetLayout({ textureDescSetLayoutBinding });

        initPipeline();

        // Atm creating these only for quick testing here!!!
        // Static vertex buffer
        float vbData[8] = {
            0, 0,
            0, -1,
            1, -1,
            1, 0
        };
        //loat vbData[16] = {
        //   xPos, yPos, 0, 1,
        //   xPos, yPos - scale, 0, 0,
        //   xPos + scale, yPos - scale, 1, 0,
        //   xPos + scale, yPos, 1, 1
        //;
        unsigned short indices[6] =
        {
            0, 1, 2,
            2, 3, 0
        };
        _pVertexBuffer = Buffer::create(
            vbData,
            sizeof(float),
            8,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT
        );
        _pIndexBuffer = Buffer::create(
            indices,
            sizeof(unsigned short),
            6,
            BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT
        );

        // Allocate "instance buffer"
        size_t totalInstanceBufLen = s_instanceBufferComponents * s_maxInstanceCount;
        float* pInitialInstanceBufData = new float[totalInstanceBufLen];
        memset(pInitialInstanceBufData, 0, sizeof(float) * totalInstanceBufLen);

        _pInstanceBuffer = Buffer::create(
            pInitialInstanceBufData,
            sizeof(float),
            totalInstanceBufLen,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT,
            true
        );
        delete[] pInitialInstanceBufData;
    }

    FontRenderer::~FontRenderer()
    {
        freeDescriptorSets();
        delete _pVertexShader;
        delete _pFragmentShader;
        delete _pVertexBuffer;
        delete _pIndexBuffer;
        delete _pInstanceBuffer;
    }

    void FontRenderer::initPipeline()
    {
        // TODO: Better way of interacting with master renderer here
        MasterRenderer* pMasterRenderer = Application::get()->getMasterRenderer();
        DescriptorSetLayout commonDescriptorSetLayout = pMasterRenderer->getCommonDescriptorSetLayout();

        std::vector<VertexBufferLayout> vbLayouts({ _vertexBufferLayout, _instanceBufferLayout });
        std::vector<DescriptorSetLayout> descSetLayouts(
            {
                commonDescriptorSetLayout,
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
            CullMode::CULL_MODE_NONE,
            FrontFace::FRONT_FACE_COUNTER_CLOCKWISE,
            true,
            DepthCompareOperation::COMPARE_OP_ALWAYS,
            sizeof(FontPushConstants), ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT
        );
    }

    void FontRenderer::submit(
        const Component* const renderableComponent,
        const mat4& transform
    )
    {
        // TODO: Delete below and add fonts to TextRenderables!
        Application* pApp = Application::get();
        ResourceManager& resManager = pApp->getResourceManager();
        std::vector<Resource*> fonts = resManager.getResourcesOfType(ResourceType::RESOURCE_FONT);
        if (fonts.empty())
            return;
        _pFont = (Font*)fonts[0];

        if (!_pFont)
        {
            Debug::log(
                "@FontRenderer::addToBatch _pFont not assigned!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        PK_id batchIdentifier = ((Resource*)_pFont)->getResourceID();
        Texture_new* pTexture = _pFont->accessTexture();

        // Create descriptor sets if necessary
        if (_descriptorSets.find(batchIdentifier) == _descriptorSets.end())
        {
            const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
            uint32_t swapchainImages = pSwapchain->getImageCount();
            std::vector<DescriptorSet*> pDescriptorSets(swapchainImages);
            for (uint32_t i = 0; i < swapchainImages; ++i)
            {
                DescriptorSet* pDescriptorSet = new DescriptorSet(
                    _textureDescSetLayout,
                    1,
                    { pTexture }
                );
                pDescriptorSets[i] = pDescriptorSet;
            }
            _descriptorSets[batchIdentifier] = { pDescriptorSets };
            Debug::log(
                "@FontRenderer::submit "
                "Descriptor sets created for new batch with id: " + std::to_string(batchIdentifier)
            );
        }

        const TextRenderable * const pRenderable = (const TextRenderable * const)renderableComponent;
        const std::string& str = pRenderable->getStr();
        if (str.size() >= s_maxInstanceCount)
        {
            Debug::log(
                "@FontRenderer::submit "
                "Attempted to submit too long string. Current limit is " + std::to_string(s_maxInstanceCount) + "\nAttempted: " + std::to_string(str.size()),
                Debug::MessageType::PK_ERROR
            );
            return;
        }

        vec2 position(transform[0 + 3 * 4], transform[1 + 3 * 4]);
        const vec3& color = pRenderable->color;

        // TODO: Quit supporting dynamic scaling -> makes text look just shit atm..
        float scaleFactorX = 1.0f; //0.625f; // 0.5f + (std::sin(s_TEST_anim) + 1.0f) * 0.5f;// 1.0f; // Atm scaling is disabled with webgl...
        float scaleFactorY = 1.0f; //0.625f; // 0.5f + (std::sin(s_TEST_anim) + 1.0f) * 0.5f;// 1.0f;

        const float originalX = position.x;
        float posX = originalX;
        float posY = position.y;

        float charWidth = _pFont->getTilePixelWidth() * scaleFactorX;
        float charHeight = _pFont->getTilePixelWidth() * scaleFactorY;

        const std::unordered_map<char, FontGlyphData>& glyphMapping = _pFont->getGlyphMapping();

        for (char c : str)
        {
            // Check, do we want to change line?
            if (c == '\n')
            {
                posY -= charHeight;
                posX = originalX;
                continue;
            }
            // Empty space uses font specific details so no any special cases here..
            else if (c == ' ')
            {
            }
            // Make sure not draw nulldtm chars accidentally..
            else if (c == 0)
            {
                break;
            }

            const FontGlyphData& glyphData = glyphMapping.at(c);

            float x = (posX + (float)glyphData.bearingX);
            // - ch because we want origin to be at 0,0 but we also need to add the bearingY so.. gets fucked without this..
            float y = posY + (float)glyphData.bearingY - charHeight;

            float renderableData[s_instanceBufferComponents] = {
                (float)(int)x, (float)(int)y,
                (float)(int)charWidth, (float)(int)charHeight,
                (float)glyphData.texOffsetX, (float)glyphData.texOffsetY,
                color.x, color.y, color.z, 1.0f
            };
            _batchContainer.addData(renderableData, sizeof(float) * s_instanceBufferComponents, batchIdentifier);

            // NOTE: Don't quite understand this.. For some reason on some fonts >> 7 works better...
            posX += ((float)(glyphData.advance >> 6)) * scaleFactorX; // now advance cursors for next glyph (note that advance is number of 1/64 pixels). bitshift by 6 to get value in pixels (2^6 = 64) | OLD COMMENT: 2^5 = 32 (pixel size of the font..)
        }
    }

    void FontRenderer::render(const Camera& cam)
    {
        if (!_pFont)
            return;

        if (!_pPipeline)
        {
            Debug::log(
                "@FontRenderer::render pipeline is nullptr",
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
                "@FontRenderer::render "
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
            pWindow->getWidth(), pWindow->getHeight(),
            0.0f, 1.0f
        );

        pRenderCmd->bindPipeline(
            pCurrentCmdBuf,
            PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
            _pPipeline
        );

        pRenderCmd->bindIndexBuffer(
            pCurrentCmdBuf,
            _pIndexBuffer,
            0,
            IndexType::INDEX_TYPE_UINT16
        );

        for (Batch* pBatch : _batchContainer.getBatches())
        {
            if (!pBatch->isOccupied())
                continue;

            // Figure out which texture using which resource
            // Quite dumb, but will do for now..

            Font* pBatchFont = (Font*)resourceManager.getResource(pBatch->getIdentifier());
            Texture_new* pBatchTexture = pBatchFont->accessTexture();
            if (!pBatchTexture)
            {
                Debug::log(
                    "@FontRenderer::render "
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

            FontPushConstants updatedPushConstants =
            {
                pCamera->getProjMat2D(),
                (float)pBatchFont->getTextureAtlasRowCount()
            };

            pRenderCmd->pushConstants(
                pCurrentCmdBuf,
                ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
                0, sizeof(FontPushConstants),
                &updatedPushConstants,
                {
                    { 0, ShaderDataType::Mat4 },
                    { 1, ShaderDataType::Float },
                }
            );

            std::unordered_map<PK_id, BatchDescriptorSets>::const_iterator descSetIt =  _descriptorSets.find(pBatch->getIdentifier());
            if (descSetIt == _descriptorSets.end())
            {
                Debug::log(
                    "@FontRenderer::render "
                    "Couldn't find descriptor sets for batchID: " + std::to_string(pBatch->getIdentifier()),
                    Debug::MessageType::PK_ERROR
                );
                continue;
            }
            // TODO: switch below [0] to [current swapchaing img index] when dealing with actual
            // swapchain stuff..
            std::vector<const DescriptorSet*> toBind =
            {
                descSetIt->second.pTextureDescriptorSet[0]
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

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            Debug::log("___TEST___GL ERR: " + std::to_string(err));
    }

    void FontRenderer::flush()
    {
        _batchContainer.clear();
    }

    void FontRenderer::freeDescriptorSets()
    {
        std::unordered_map<PK_id, BatchDescriptorSets>::iterator it;
        for (it = _descriptorSets.begin(); it != _descriptorSets.end(); ++it)
        {
            for (DescriptorSet* pDescriptorSet : it->second.pTextureDescriptorSet)
                delete pDescriptorSet;
            it->second.pTextureDescriptorSet.clear();
        }
        _descriptorSets.clear();
    }
}
