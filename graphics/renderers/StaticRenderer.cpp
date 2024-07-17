#include "StaticRenderer.h"
#include <string>
#include "core/Application.h"
#include "ecs/components/renderable/Static3DRenderable.h"

#include <GL/glew.h>


namespace pk
{

    // NOTE: Atm these here only for quick testing!
    static std::string s_vertexSource = R"(
        precision mediump float;

        attribute vec3 vertexPos;
        attribute vec2 uvCoord;

        struct Common3D
        {
            mat4 projMat;
            mat4 viewMat;
        };
        struct Renderable
        {
            mat4 transformationMat;
        };

        uniform Common3D common;
        uniform Renderable renderable;

        varying vec2 var_uvCoord;

        void main()
        {
            gl_Position = common.projMat * common.viewMat * renderable.transformationMat * vec4(vertexPos, 1.0);
            var_uvCoord = uvCoord;
        }
    )";

    static std::string s_fragmentSource = R"(
        precision mediump float;

        varying vec2 var_uvCoord;

        uniform sampler2D texSampler;

        void main()
        {
            vec4 texColor = texture2D(texSampler, var_uvCoord);
            gl_FragColor = texColor;
        }
    )";

    StaticRenderer::StaticRenderer() :
        _vertexBufferLayout({}, VertexInputRate::VERTEX_INPUT_RATE_INSTANCE),
        _UBODescSetLayout({}),
        _textureDescSetLayout({})
    {
        _pVertexShader = Shader::create(s_vertexSource, ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT);
        _pFragmentShader = Shader::create(s_fragmentSource, ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT);

        // Vertex buffer layouts
        _vertexBufferLayout = VertexBufferLayout(
            {
                { 0, ShaderDataType::Float3 },
                { 1, ShaderDataType::Float2 }
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        );

        // ubo(atm just transformation matrix) desc set layout
        DescriptorSetLayoutBinding uboDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
            {{ 2, ShaderDataType::Mat4 }}
        );
        _UBODescSetLayout = DescriptorSetLayout({ uboDescSetLayoutBinding });
        // Also atm create single(x maxSwapchainImages) ubo for testing the transform desc set here..
        _pTransformUBO.resize(MAX_SWAPCHAIN_IMAGES);
        float initialUBOBuf[16];
        memset(initialUBOBuf, 0, 16);
        for (int i = 0; i < MAX_SWAPCHAIN_IMAGES; ++i)
        {
            _pTransformUBO[i] = Buffer::create(
                initialUBOBuf,
                sizeof(mat4),
                1,
                BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                true
            );
        }

        // Textures desc set layout
        DescriptorSetLayoutBinding textureDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 3 }}
        );
        _textureDescSetLayout = DescriptorSetLayout({ textureDescSetLayoutBinding });

        initPipeline();

        // Atm creating these only for quick testing here!!!
        // Static vertex buffer
        float scale = 10;
        float vbData[20] = {
            -1 * scale,  1 * scale, -1.0f,   0, 1,
            -1 * scale, -1 * scale, -1.0f,   0, 0,
             1 * scale, -1 * scale, -1.0f,   1, 0,
             1 * scale,  1 * scale, -1.0f,   1, 1
        };
        unsigned short indices[6] =
        {
            0, 1, 2,
            2, 3, 0
        };
        _pVertexBuffer = Buffer::create(
            vbData,
            sizeof(float),
            20,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT
        );
        _pIndexBuffer = Buffer::create(
            indices,
            sizeof(unsigned short),
            6,
            BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT
        );
    }

    StaticRenderer::~StaticRenderer()
    {
        freeDescriptorSets();
        delete _pVertexShader;
        delete _pFragmentShader;
        delete _pVertexBuffer;
        delete _pIndexBuffer;
    }

    void StaticRenderer::initPipeline()
    {
        const MasterRenderer* pMasterRenderer = Application::get()->getMasterRenderer();
        std::vector<VertexBufferLayout> vbLayouts({ _vertexBufferLayout });
        std::vector<DescriptorSetLayout> descSetLayouts(
            {
                pMasterRenderer->getCommonDescriptorSetLayout(),
                _UBODescSetLayout,
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
        size_t swapchainImages = pApp->getWindow()->getSwapchain()->getImageCount();

        const Static3DRenderable * const pStaticRenderable = (const Static3DRenderable * const)renderableComponent;
        Mesh* pMesh = (Mesh*)resManager.getResource(pStaticRenderable->meshID);
        Material* pMaterial = pMesh->accessMaterial();
        Texture_new* pTestTexture = pMaterial->accessTexture(0);

        if (pMesh && pMaterial)
        {
            // create descriptor sets if necessary
            // FUCKING DISGUSTING! DO SOMETHING ABOUT THIS!!!
            if (_pUBODescriptorSet.empty())
            {
                std::vector<DescriptorSet*> pDescriptorSets(swapchainImages);
                for (uint32_t i = 0; i < swapchainImages; ++i)
                {
                    DescriptorSet* pDescriptorSet = new DescriptorSet(
                        _UBODescSetLayout,
                        1,
                        { _pTransformUBO }
                    );
                    pDescriptorSets[i] = pDescriptorSet;
                }
                _pUBODescriptorSet = pDescriptorSets;
                Debug::log("___TEST___created ubo desc set for 3d renderable");
            }
            if (_pTextureDescriptorSet.empty())
            {
                std::vector<DescriptorSet*> pDescriptorSets(swapchainImages);
                for (uint32_t i = 0; i < swapchainImages; ++i)
                {
                    DescriptorSet* pDescriptorSet = new DescriptorSet(
                        _textureDescSetLayout,
                        1,
                        { pTestTexture }
                    );
                    pDescriptorSets[i] = pDescriptorSet;
                }
                _pTextureDescriptorSet = pDescriptorSets;
                Debug::log("___TEST___created tex desc set for 3d renderable");
            }
        }
        // update ubo
        _pTransformUBO[0]->update(transformation.getRawArray(), sizeof(mat4));
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

        pRenderCmd->bindIndexBuffer(
            pCurrentCmdBuf,
            _pIndexBuffer,
            0,
            IndexType::INDEX_TYPE_UINT16
        );

        std::vector<Buffer*> vertexBuffers = { _pVertexBuffer };
        pRenderCmd->bindVertexBuffers(
            pCurrentCmdBuf,
            0, 1,
            vertexBuffers
        );

        // Get "common descriptor set" from master renderer
        MasterRenderer* pMasterRenderer = pApp->getMasterRenderer();
        const DescriptorSet* pCommonDescriptorSet = pMasterRenderer->getCommonDescriptorSet();

        std::vector<const DescriptorSet*> toBind =
        {
            pCommonDescriptorSet,
            _pUBODescriptorSet[0],
            _pTextureDescriptorSet[0]
        };

        pRenderCmd->bindDescriptorSets(
            pCurrentCmdBuf,
            PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
            0,
            toBind
        );

        pRenderCmd->drawIndexed(pCurrentCmdBuf, 6, 1, 0, 0, 0);


        pRenderCmd->endCmdBuffer(pCurrentCmdBuf);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            Debug::log("___TEST___GL ERR: " + std::to_string(err));
    }

    void StaticRenderer::flush()
    {
    }

    void StaticRenderer::freeDescriptorSets()
    {
        for (DescriptorSet* pDescSet : _pUBODescriptorSet)
            delete pDescSet;
        _pUBODescriptorSet.clear();
        for (DescriptorSet* pDescSet : _pTextureDescriptorSet)
            delete pDescSet;
        _pTextureDescriptorSet.clear();
    }
}
