#include "GUIRenderer.h"
#include <string>
#include "core/Application.h"

#include <GL/glew.h>


namespace pk
{
    struct CommonUBO
    {
        mat4 projMat;
        vec3 color;
    };

    // NOTE: Atm these here only for quick testing!
    static std::string s_vertexSource = R"(
        precision mediump float;

        attribute vec2 position;
        attribute vec2 uvCoord;

        struct Common
        {
            mat4 projMat;
            vec3 color;
        };

        uniform Common common;

        varying vec3 test;
        varying vec2 var_uvCoord;

        void main()
        {
            gl_Position = common.projMat * vec4(position, 0, 1.0);
            test = common.color;
            var_uvCoord = uvCoord;
        }
    )";

    static std::string s_fragmentSource = R"(
        precision mediump float;

        varying vec3 test;
        varying vec2 var_uvCoord;

        uniform sampler2D texSampler;

        void main()
        {
            vec4 texColor = texture2D(texSampler, var_uvCoord);
            gl_FragColor = texColor;
        }
    )";


    GUIRenderer::GUIRenderer()
    {
        _pVertexShader = Shader::create(s_vertexSource, ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT);
        _pFragmentShader = Shader::create(s_fragmentSource, ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT);

        VertexBufferLayout vbLayout(
            {
                { 0, ShaderDataType::Float2 },
                { 1, ShaderDataType::Float2 }
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        );

        // UBO desc set layout
        DescriptorSetLayoutBinding uboDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
            {
                { 0, ShaderDataType::Mat4 },
                { 1, ShaderDataType::Float3 }
            }
        );
        DescriptorSetLayout uboDescSetLayout({uboDescSetLayoutBinding});

        // Textures desc set layout
        DescriptorSetLayoutBinding textureDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 2 }}
        );
        DescriptorSetLayout textureDescSetLayout({textureDescSetLayoutBinding});

        std::vector<VertexBufferLayout> vbLayouts({vbLayout});
        std::vector<DescriptorSetLayout> descSetLayouts({uboDescSetLayout, textureDescSetLayout});

        const Window* pWindow = Application::get()->getWindow();
        int windowWidth = pWindow->getWidth();
        int windowHeight = pWindow->getHeight();

        _pPipeline = Pipeline::create(
            vbLayouts,
            descSetLayouts,
            _pVertexShader, _pFragmentShader,
            (float)windowWidth, (float)windowHeight,
            { 0, 0, (uint32_t)windowWidth, (uint32_t)windowHeight },
            CullMode::CULL_MODE_NONE,
            FrontFace::FRONT_FACE_COUNTER_CLOCKWISE,
            true,
            DepthCompareOperation::COMPARE_OP_ALWAYS
        );

        _pCmdBuf = CommandBuffer::create();

        _pRenderCommand = RenderCommand::create();

        // Atm creating these only for quick testing here!!!
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

        // Test UBOs
        CommonUBO initialCommon;
        _pTestUBO = Buffer::create(
            &initialCommon,
            sizeof(CommonUBO),
            1,
            BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT
        );

        _pUBODescriptorSet = new DescriptorSet(
            uboDescSetLayout,
            1,
            { _pTestUBO }
        );

        // Test textures
        TextureSampler texSampler;
        ImageData* pTexImg = load_image("assets/vulcanic.png");
        _pTestTexture = Texture_new::create(texSampler, pTexImg);

        _pTextureDescriptorSet = new DescriptorSet(
            textureDescSetLayout,
            1,
            { _pTestTexture }
        );
    }

    GUIRenderer::~GUIRenderer()
    {
        delete _pVertexShader;
        delete _pFragmentShader;
        delete _pTestUBO;
        delete _pUBODescriptorSet;
        delete _pTestTexture;
        delete _pTextureDescriptorSet;
        delete _pCmdBuf;
        delete _pRenderCommand;
    }

    void GUIRenderer::update()
    {
        //glClear(GL_COLOR_BUFFER_BIT);
        //glClearColor(0, 0, 1, 1);

        // Update current local projMatUbo with camera
        Camera* pSceneCamera = Application::get()->getCurrentScene()->activeCamera;
        const mat4 projMat = pSceneCamera->getProjMat2D();

        float r = (std::rand() % 255) / 255.0f;
        float g = (std::rand() % 255) / 255.0f;
        float b = (std::rand() % 255) / 255.0f;
        vec3 randomColor(r, g, b);
        CommonUBO commonUBO = { projMat, randomColor };
        _pTestUBO->update(&commonUBO, sizeof(commonUBO));

        _pRenderCommand->beginCmdBuffer(_pCmdBuf);

        const Window * const pWindow = Application::get()->getWindow();

        _pRenderCommand->setViewport(
            _pCmdBuf,
            0, 0,
            pWindow->getWidth(), pWindow->getHeight(),
            0.0f, 1.0f
        );

        _pRenderCommand->bindPipeline(
            _pCmdBuf,
            PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
            _pPipeline
        );

        _pRenderCommand->bindIndexBuffer(
            _pCmdBuf,
            _pIndexBuffer,
            0,
            IndexType::INDEX_TYPE_UINT16
        );


        std::vector<Buffer*> vertexBuffers = { _pVertexBuffer };
        _pRenderCommand->bindVertexBuffers(
            _pCmdBuf,
            0, 1,
            vertexBuffers
        );

        std::vector<DescriptorSet*> descriptorSets = { _pUBODescriptorSet, _pTextureDescriptorSet };
        _pRenderCommand->bindDescriptorSets(
            _pCmdBuf,
            PipelineBindPoint::PIPELINE_BIND_POINT_GRAPHICS,
            0,
            descriptorSets
        );

        _pRenderCommand->drawIndexed(_pCmdBuf, 6, 1, 0, 0, 0);

        _pRenderCommand->endCmdBuffer(_pCmdBuf);


        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            Debug::log("___TEST___GL ERR: " + std::to_string(err));
    }
}
