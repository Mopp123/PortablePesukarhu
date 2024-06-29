#include "GUIRenderer.h"
#include <string>
#include "core/Application.h"
#include "ecs/components/renderable/GUIRenderable.h"

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

        void main()
        {
            vec4 texColor = texture2D(texSampler, var_uvCoord);
            gl_FragColor = texColor;
        }
    )";


    GUIRenderer::GUIRenderer() :
        _textureDescSetLayout({})
    {
        _pVertexShader = Shader::create(s_vertexSource, ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT);
        _pFragmentShader = Shader::create(s_fragmentSource, ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT);

        // Textures desc set layout
        DescriptorSetLayoutBinding textureDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 1 }}
        );
        _textureDescSetLayout = DescriptorSetLayout({ textureDescSetLayoutBinding });

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
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT,
            true
        );
    }

    GUIRenderer::~GUIRenderer()
    {
        delete _pVertexShader;
        delete _pFragmentShader;
        delete _pVertexBuffer;
        delete _pInstancedVertexBuffer;
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


        // TODO: Better way of interacting with master renderer here
        MasterRenderer* pMasterRenderer = Application::get()->getMasterRenderer();
        DescriptorSetLayout commonDescriptorSetLayout = pMasterRenderer->getCommonDescriptorSetLayout();


        std::vector<VertexBufferLayout> vbLayouts({ vbLayout, instancedVbLayout });
        std::vector<DescriptorSetLayout> descSetLayouts({commonDescriptorSetLayout, _textureDescSetLayout});

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

        std::vector<const DescriptorSet*> descriptorSets;
        // TODO: On gui rendering get projection matrix and use it as push constant instead of descriptor set
        // since "CommonUniforms" will eventually have more stuff, gui rendering wont use!
        const DescriptorSet* pCommonDescriptorSet = Application::get()->getMasterRenderer()->getCommonDescriptorSet();
        descriptorSets.push_back(pCommonDescriptorSet);

        // FOR TESTING: only using first texture descriptor set..
        if (_textureDescriptorSets.size() > 0)
        {
            std::unordered_map<Texture_new*, std::vector<DescriptorSet*>>::const_iterator it = _textureDescriptorSets.begin();
            if ((*it).second.size() > 0)
            {
                DescriptorSet* pDescriptorSet = (*it).second[0];
                descriptorSets.push_back(pDescriptorSet);
            }
        }
        //std::vector<const DescriptorSet*> descriptorSets = { pCommonDescriptorSet, _pTextureDescriptorSet };
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

    void GUIRenderer::createDescriptorSets(Component* pComponent)
    {
        Texture_new* pTexture = ((GUIRenderable*)pComponent)->pTexture_new;
        if (pTexture)
        {
            if (_textureDescriptorSets.find(pTexture) == _textureDescriptorSets.end())
            {
                const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
                for (int i = 0; i < pSwapchain->getImageCount(); ++i)
                {
                    DescriptorSet* pDescriptorSet = new DescriptorSet(
                        _textureDescSetLayout,
                        1,
                        { pTexture }
                    );
                    _textureDescriptorSets[pTexture].push_back(pDescriptorSet);
                }
            }
        }
    }

    void GUIRenderer::freeDescriptorSets()
    {
        std::unordered_map<Texture_new*, std::vector<DescriptorSet*>>::iterator it;
        for (it = _textureDescriptorSets.begin(); it != _textureDescriptorSets.end(); ++it)
        {
            for (DescriptorSet* pDescriptorSet : (*it).second)
                delete pDescriptorSet;
            (*it).second.clear();
        }
    }

    void GUIRenderer::recreateDescriptorSets()
    {
        // NOTE: This does not touch the already created "Texture - keys"
        // in the _textureDescriptorSets map! It only deletes the actual
        // descriptor sets and empties the vector they are contained in!
        freeDescriptorSets();
        // Create new descriptor sets for each existing key
        std::unordered_map<Texture_new*, std::vector<DescriptorSet*>>::iterator it;
        const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
        for (it = _textureDescriptorSets.begin(); it != _textureDescriptorSets.end(); ++it)
        {
            Texture_new* pTexture = (*it).first;
            for (int i = 0; i < pSwapchain->getImageCount(); ++i)
            {
                DescriptorSet* pDescriptorSet = new DescriptorSet(
                    _textureDescSetLayout,
                    1,
                    { pTexture }
                );
                _textureDescriptorSets[pTexture].push_back(pDescriptorSet);
            }
        }
        Debug::log("___TEST___GUI RENDERER RECREATED DESCRIPTOR SETS!");
    }
}
