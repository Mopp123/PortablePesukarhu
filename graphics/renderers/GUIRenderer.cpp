#include "GUIRenderer.h"
#include <string>
#include "core/Application.h"

#include <GL/glew.h>


namespace pk
{

    // NOTE: Atm these here only for quick testing!
    static std::string s_vertexSource = R"(
        precision mediump float;

        attribute vec2 vertexPos;
        attribute vec2 uvCoord;

        attribute vec2 screenPos;

        struct Common
        {
            mat4 projMat;
        };

        uniform Common common;

        varying vec2 var_uvCoord;

        void main()
        {
            gl_Position = common.projMat * vec4(vertexPos + screenPos, 0, 1.0);
            var_uvCoord = uvCoord;
        }
    )";

    static std::string s_fragmentSource = R"(
        precision mediump float;

        varying vec2 var_uvCoord;

        uniform sampler2D texSampler;
        uniform sampler2D texSampler2;

        void main()
        {
            vec4 texColor = texture2D(texSampler, var_uvCoord);
            vec4 texColor2 = texture2D(texSampler2, var_uvCoord);
            gl_FragColor = mix(texColor, texColor2, 0.5);
        }
    )";


    GUIRenderer::GUIRenderer() :
        _uboDescSetLayout({}),
        _textureDescSetLayout({})
    {
        _pVertexShader = Shader::create(s_vertexSource, ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT);
        _pFragmentShader = Shader::create(s_fragmentSource, ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT);

        initPipeline();

        const Window* pWindow = Application::get()->getWindow();
        // Atm creating these only for quick testing here!!!
        // Static vertex buffer
        float yPos = pWindow->getHeight() - 10;
        float xPos = 10;
        float scale = 200;
        float vbData[16] = {
            xPos, yPos, 0, 1,
            xPos, yPos - scale, 0, 0,
            xPos + scale, yPos - scale, 1, 0,
            xPos + scale, yPos, 1, 1
        };
        unsigned short indices[6] =
        {
            0, 1, 2,
            2, 3, 0
        };
        _pVertexBuffer = Buffer::create(
            vbData,
            sizeof(float),
            16,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT
        );
        _pIndexBuffer = Buffer::create(
            indices,
            sizeof(unsigned short),
            6,
            BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT
        );

        // instanced vertex buffer
        float instancedBuf[14] =
        {
            0, -100,
            (float)(std::rand() % 200), -100,
            (float)(std::rand() % 200), -100,
            (float)(std::rand() % 200), -100,
            (float)(std::rand() % 200), -100,
            (float)(std::rand() % 200), -100,
            (float)(std::rand() % 200), -100
        };
        _pInstancedVertexBuffer = Buffer::create(
            instancedBuf,
            sizeof(float),
            14,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT
        );

        // Test UBOs
        CommonUBO initialCommon;
        _pTestUBO = Buffer::create(
            &initialCommon,
            sizeof(CommonUBO),
            1,
            BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT
        );

        _pUBODescriptorSet = new DescriptorSet(
            _uboDescSetLayout,
            1,
            { _pTestUBO }
        );

        // Test textures
        TextureSampler texSampler;

        ImageData* pTexImg = load_image("assets/vulcanic.png");
        _pTestTexture = Texture_new::create(texSampler, pTexImg);

        ImageData* pTexImg2 = load_image("assets/asd.png");
        _pTestTexture2 = Texture_new::create(texSampler, pTexImg2);

        _pTextureDescriptorSet = new DescriptorSet(
            _textureDescSetLayout,
            2,
            { _pTestTexture, _pTestTexture2 }
        );
    }

    GUIRenderer::~GUIRenderer()
    {
        delete _pVertexShader;
        delete _pFragmentShader;
        delete _pVertexBuffer;
        delete _pInstancedVertexBuffer;
        delete _pTestUBO;
        delete _pUBODescriptorSet;
        delete _pTestTexture;
        delete _pTestTexture2;
        delete _pTextureDescriptorSet;
    }

    void GUIRenderer::initPipeline()
    {
        // Vertex buffer layouts
        VertexBufferLayout vbLayout(
            {
                { 0, ShaderDataType::Float2 },
                { 1, ShaderDataType::Float2 }
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        );
        VertexBufferLayout instancedVbLayout(
            {
                { 2, ShaderDataType::Float2 },
            },
            VertexInputRate::VERTEX_INPUT_RATE_INSTANCE
        );


        // UBO desc set layout
        DescriptorSetLayoutBinding uboDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
            {
                { 0, ShaderDataType::Mat4 },
            }
        );
        _uboDescSetLayout = DescriptorSetLayout({uboDescSetLayoutBinding});

        // Textures desc set layout
        DescriptorSetLayoutBinding textureDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 1 }}
        );
        DescriptorSetLayoutBinding textureDescSetLayoutBinding2(
            1,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 2 }}
        );
        _textureDescSetLayout = DescriptorSetLayout({ textureDescSetLayoutBinding, textureDescSetLayoutBinding2 });

        std::vector<VertexBufferLayout> vbLayouts({ vbLayout, instancedVbLayout });
        std::vector<DescriptorSetLayout> descSetLayouts({_uboDescSetLayout, _textureDescSetLayout});

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
            DepthCompareOperation::COMPARE_OP_ALWAYS
        );
    }

    void GUIRenderer::submit(const Component* const renderableComponent, const mat4& transformation)
    {
    }

    void GUIRenderer::render(const Camera& cam)
    {
        if (!_pPipeline)
        {
            Debug::log(
                "@GUIRenderer::render pipeline is nullptr",
                Debug::MessageType::PK_ERROR
            );
        }
        CommandBuffer* pCurrentCmdBuf = _pCommandBuffers[RenderPassType::RENDER_PASS_DIFFUSE][0];

        // Update current local projMatUbo with camera
        Camera* pSceneCamera = Application::get()->getCurrentScene()->activeCamera;
        const mat4 projMat = pSceneCamera->getProjMat2D();

        CommonUBO commonUBO = { projMat };
        _pTestUBO->update(&commonUBO, sizeof(commonUBO));

        RenderCommand* pRenderCmd = RenderCommand::get();

        pRenderCmd->beginCmdBuffer(pCurrentCmdBuf);

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


        // test updating instanced buffer
        s_testX += Timing::get_delta_time();
        float testPosX = 1.0f + (std::cos(s_testX) * 100.0f);
        float testPosX2 = 1.0f + (std::sin(s_testX) * 100.0f);
        float updatedInstancedBuf[14] =
        {
            testPosX,  -100,
            testPosX2, -200,
            testPosX,  -300,
            testPosX2, -400,
            testPosX,  -500,
            testPosX2, -600,
            testPosX,  -700
        };
        _pInstancedVertexBuffer->update((const void*)updatedInstancedBuf, sizeof(float) * 14);

        std::vector<Buffer*> vertexBuffers = { _pVertexBuffer, _pInstancedVertexBuffer };
        pRenderCmd->bindVertexBuffers(
            pCurrentCmdBuf,
            0, 2,
            vertexBuffers
        );

        std::vector<DescriptorSet*> descriptorSets = { _pUBODescriptorSet, _pTextureDescriptorSet };
        pRenderCmd->bindDescriptorSets(
            pCurrentCmdBuf,
            PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
            0,
            descriptorSets
        );

        pRenderCmd->drawIndexed(pCurrentCmdBuf, 6, 7, 0, 0, 0);

        pRenderCmd->endCmdBuffer(pCurrentCmdBuf);


        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            Debug::log("___TEST___GL ERR: " + std::to_string(err));
    }
}
