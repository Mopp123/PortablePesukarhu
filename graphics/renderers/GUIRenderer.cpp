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

        attribute vec2 pos;
        attribute vec2 scale;

        struct Common
        {
            mat4 projMat;
        };

        uniform Common common;

        varying vec2 var_uvCoord;

        void main()
        {
            gl_Position = common.projMat * vec4((vertexPos * scale) + pos, 0, 1.0);
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
        _vertexBufferLayout({}, VertexInputRate::VERTEX_INPUT_RATE_INSTANCE),
        _instanceBufferLayout({}, VertexInputRate::VERTEX_INPUT_RATE_INSTANCE),
        _textureDescSetLayout({}),
        _batchContainer(2, (sizeof(float) * 4) * 400)
    {
        _pVertexShader = Shader::create(s_vertexSource, ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT);
        _pFragmentShader = Shader::create(s_fragmentSource, ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT);

        // Vertex buffer layouts
        _vertexBufferLayout = VertexBufferLayout(
            {
                { 0, ShaderDataType::Float2 },
                { 1, ShaderDataType::Float2 }
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        );
        _instanceBufferLayout = VertexBufferLayout(
            {
                { 2, ShaderDataType::Float2 }, // pos
                { 3, ShaderDataType::Float2 }  // scale
            },
            VertexInputRate::VERTEX_INPUT_RATE_INSTANCE
        );

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
            0, 0, 0, 1,
            0, -1, 0, 0,
            1, -1, 1, 0,
            1, 0, 1, 1
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
            16,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT
        );
        _pIndexBuffer = Buffer::create(
            indices,
            sizeof(unsigned short),
            6,
            BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT
        );

        // Allocate "instance buffer"
        size_t maxInstanceCount = 100;
        uint32_t instanceBufElemLen = 4;
        size_t totalInstanceBufLen = instanceBufElemLen * maxInstanceCount;
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

    GUIRenderer::~GUIRenderer()
    {
        delete _pVertexShader;
        delete _pFragmentShader;
        delete _pVertexBuffer;
        delete _pIndexBuffer;
        delete _pInstanceBuffer;
    }

    void GUIRenderer::initPipeline()
    {
        // TODO: Better way of interacting with master renderer here
        MasterRenderer* pMasterRenderer = Application::get()->getMasterRenderer();
        DescriptorSetLayout commonDescriptorSetLayout = pMasterRenderer->getCommonDescriptorSetLayout();

        std::vector<VertexBufferLayout> vbLayouts({ _vertexBufferLayout, _instanceBufferLayout });
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
        vec2 pos(transformation[0 + 3 * 4], transformation[1 + 3 * 4]);
        vec2 scale(transformation[0 + 0 * 4], transformation[1 + 1 * 4]);

        //float newData[4] = { pos.x, pos.y, scale.x, scale.y };
        //_pInstanceBuffer->update(newData, sizeof(float) * 4);

        const GUIRenderable * const pGuiRenderable = (const GUIRenderable * const)renderableComponent;
        PK_id batchIdentifier = pGuiRenderable->pTexture_new->getResourceID();

        float renderableData[4] = { pos.x, pos.y, scale.x, scale.y };
        _batchContainer.addData(renderableData, sizeof(float) * 4, batchIdentifier);
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

        //const std::unordered_map<PK_id, Batch*>& batches = _batchContainer.getOccupiedBatches();
        //std::unordered_map<PK_id, Batch*>::const_iterator batchIt;
        for (Batch* pBatch : _batchContainer.getBatches())
        {
            if (!pBatch->isOccupied())
                continue;

            Buffer* pBatchInstanceBuffer = pBatch->getBuffer();
            std::vector<Buffer*> vertexBuffers = { _pVertexBuffer, pBatchInstanceBuffer };
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

            pRenderCmd->drawIndexed(pCurrentCmdBuf, 6, pBatch->getInstanceCount(), 0, 0, 0);
        }

        pRenderCmd->endCmdBuffer(pCurrentCmdBuf);

        _batchContainer.clear();

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
