#include "SkinnedRenderer.h"
#include <string>
#include "core/Application.h"
#include "ecs/components/renderable/SkinnedRenderable.h"

#include <GL/glew.h>
#include "graphics/platform/opengl/OpenglContext.h"


namespace pk
{
    static void update_joint_matrices(
        Scene* pScene,
        entityID_t jointEntity,
        int currentJointIndex,
        std::vector<mat4>& matrices,
        const Pose& bindPose
    )
    {
        Transform* pTransform = (Transform*)pScene->getComponent(jointEntity, ComponentType::PK_TRANSFORM);
        const mat4& inverseBindMatrix = bindPose.joints[currentJointIndex].inverseMatrix;
        matrices[currentJointIndex] = pTransform->accessTransformationMatrix() * inverseBindMatrix;

        std::vector<entityID_t> childJointEntities = pScene->getChildren(jointEntity);
        for (int i = 0; i < bindPose.jointChildMapping[currentJointIndex].size(); ++i)
        {
            int childJointIndex = bindPose.jointChildMapping[currentJointIndex][i];
            update_joint_matrices(
                pScene,
                childJointEntities[i],
                childJointIndex,
                matrices,
                bindPose
            );
        }
    }

    // TODO: Make it possible on "client side project" to set initial
    // size on "init/scene switch stage"
    SkinnedMeshBatch::SkinnedMeshBatch(
        const Material* pMaterial,
        const vec4& materialProperties,
        size_t initialSize,
        const DescriptorSetLayout& materialDescriptorSetLayout,
        // NOTE: using same ubo for all batches
        //  ->we only change the data of the ubo to match batch when we render the batch
        const std::vector<const Buffer*>& pMaterialPropsUniformBuffers
    ) :
        pMaterial(pMaterial),
        materialProperties(materialProperties),
        initialSize(initialSize)
    {
        transformationMatrices.resize(initialSize);
        rootJoints.resize(initialSize);

        const Texture_new* pDiffuseTexture = pMaterial->getDiffuseTexture(0);
        const Texture_new* pSpecularTexture = pMaterial->getSpecularTexture();

        const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
        uint32_t swapchainImages = pSwapchain->getImageCount();
        materialDescriptorSet.resize(swapchainImages);
        for (int i = 0; i < swapchainImages; ++i)
        {
            materialDescriptorSet[i] = new DescriptorSet(
                materialDescriptorSetLayout,
                1, // NOTE: still dont remember was this supposed to be descriptor count or descriptor SET count.. -> May fuck up shit in future!
                { pDiffuseTexture, pSpecularTexture },
                pMaterialPropsUniformBuffers
            );
        }
    }

    SkinnedMeshBatch::~SkinnedMeshBatch()
    {
        for (DescriptorSet* pMaterialDescriptorSet : materialDescriptorSet)
            delete pMaterialDescriptorSet;
    }

    void SkinnedMeshBatch::add(const mat4& transformationMatrix, entityID_t rootJoint)
    {
        if (occupiedCount + 1 >= transformationMatrices.size())
        {
            transformationMatrices.push_back(transformationMatrix);
            rootJoints.push_back(rootJoint);
        }
        else
        {
            transformationMatrices[occupiedCount] = transformationMatrix;
            rootJoints[occupiedCount] = rootJoint;
        }
        ++occupiedCount;
    }

    // NOTE: We dont clear transformationMatrices or rootJoints here -> we only zero it
    // because we can be quite sure that their sizes remains the same on
    // next frame as well
    void SkinnedMeshBatch::clear()
    {
        pMaterial = nullptr;
        materialProperties = { 0.0f, 0.0f, 0.0f, 0.0f };
        memset(
            transformationMatrices.data(),
            0,
            sizeof(mat4) * transformationMatrices.size()
        );
        memset(
            rootJoints.data(),
            0,
            sizeof(entityID_t) * rootJoints.size()
        );
        occupiedCount = 0;
    }


    static int s_maxJoints = 50;
    SkinnedRenderer::SkinnedRenderer() :
        _vertexBufferLayout(
            {
                { 0, ShaderDataType::Float3 },
                { 1, ShaderDataType::Float3 },
                { 2, ShaderDataType::Float2 },

                // Weights
                { 3, ShaderDataType::Float4 },
                // Joints
                { 4,  ShaderDataType::Float4 },
            },
            VertexInputRate::VERTEX_INPUT_RATE_VERTEX
        ),
        _materialDescSetLayout({}),
        _jointDescriptorSetLayout({})
    {
        _pVertexShader = Shader::create_from_file(
            "assets/shaders/SkinnedShader.vert",
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT
        );
        _pFragmentShader = Shader::create_from_file(
            "assets/shaders/SkinnedShader.frag",
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT
        );

        // Joint matrices desc set layout
        _jointDescriptorSetLayout = DescriptorSetLayout(
            {
                {
                    0,
                    1,
                    DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
                    {{ 6, ShaderDataType::Mat4, s_maxJoints }}
                }
            }
        );
        // Create joint matrices uniform buffer(s) and descriptor set(s)
        const Swapchain* pSwapchain = Application::get()->getWindow()->getSwapchain();
        uint32_t swapchainImages = pSwapchain->getImageCount();
        createJointUniformBuffers(swapchainImages);
        createJointDescriptorSets(_jointMatricesBuffer);

        // Material desc set layout
        // Diffuse texture
        DescriptorSetLayoutBinding diffTextureDescSetLayoutBinding(
            0,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 56 }}
        );
        // Specular texture
        DescriptorSetLayoutBinding specTextureDescSetLayoutBinding(
            1,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 57 }}
        );
        // "properties"
        DescriptorSetLayoutBinding materialPropsDescSetLayoutBinding(
            2,
            1,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 58, ShaderDataType::Float4 }}
        );


        _materialDescSetLayout = DescriptorSetLayout(
            {
                diffTextureDescSetLayoutBinding,
                specTextureDescSetLayoutBinding,
                materialPropsDescSetLayoutBinding
            }
        );

        // Create material "properties" ubo
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

    SkinnedRenderer::~SkinnedRenderer()
    {
        freeDescriptorSets();
        delete _pVertexShader;
        delete _pFragmentShader;
    }

    void SkinnedRenderer::initPipeline()
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
            sizeof(mat4), // sending transformation matrices as push constants
            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT
        );
    }

    void SkinnedRenderer::submit(
        const Component* const renderableComponent,
        const mat4& transformation
    )
    {
        const Application* pApp = Application::get();
        const ResourceManager& resManager = pApp->getResourceManager();

        const SkinnedRenderable * const pRenderable = (const SkinnedRenderable * const)renderableComponent;
        PK_id batchIdentifier = pRenderable->meshID;
        const Mesh* pMesh = (const Mesh*)resManager.getResource(pRenderable->meshID);
        if (!pMesh)
        {
            Debug::log(
                "@SkinnedRenderer::submit "
                "Renderable had invalid mesh",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        const Material* pMaterial = pMesh->getMaterial();

        if (_identifierBatchMapping.find(batchIdentifier) != _identifierBatchMapping.end())
        {
            int batchIndex = _identifierBatchMapping[batchIdentifier];
            if (batchIndex >= _batches.size())
            {
                Debug::log(
                    "@SkinnedRenderer::submit "
                    "batch index: " + std::to_string(batchIndex) + " "
                    "was out of bounds for found batch identifier: " + std::to_string(batchIdentifier),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            SkinnedMeshBatch* pBatch = _batches[batchIndex];
            pBatch->add(transformation, pRenderable->skeletonEntity);
        }
        else
        {
            vec4 materialProperties(
                pMaterial->getSpecularStrength(),
                pMaterial->getSpecularShininess(),
                0.0f,
                0.0f
            );
            SkinnedMeshBatch* pNewBatch = new SkinnedMeshBatch(
                pMaterial,
                materialProperties,
                32,
                _materialDescSetLayout,
                _constMaterialPropsUniformBuffers
            );
            pNewBatch->add(transformation, pRenderable->skeletonEntity);
            _batches.push_back(pNewBatch);
            _identifierBatchMapping[batchIdentifier] = _batches.size() - 1;

            // Just temp hack to test this stuff..
            if (_batchModelIDMapping.find(batchIdentifier) == _batchModelIDMapping.end())
                _batchModelIDMapping[batchIdentifier] = pRenderable->modelID;
        }
    }

    void SkinnedRenderer::render()
    {
        if (!_pPipeline)
        {
            Debug::log(
                "@SkinnedRenderer::render pipeline is nullptr",
                Debug::MessageType::PK_ERROR
            );
        }
        Application* pApp = Application::get();
        // If joint matrices ubos and/or descriptor sets were deleted for some reason
        //  -> recreate
        const Swapchain* pSwapchain = pApp->getWindow()->getSwapchain();
        uint32_t swapchainImages = pSwapchain->getImageCount();
        if (_jointMatricesBuffer.empty())
        {
            createJointUniformBuffers(swapchainImages);
            Debug::log(
                "@SkinnedRenderer::render "
                "Joint matrices uniform buffers recreated. "
                "New count: " + std::to_string(_jointMatricesBuffer.size())
            );
        }
        if (_jointDescriptorSet.empty())
        {
            createJointDescriptorSets(_jointMatricesBuffer);
            Debug::log(
                "@SkinnedRenderer::render "
                "Joint matrices descriptor sets recreated. "
                "New count: " + std::to_string(_jointDescriptorSet.size())
            );
        }

        Scene* pCurrentScene = pApp->accessCurrentScene();
        entityID_t cameraID = pCurrentScene->activeCamera;
        Camera* pCamera = (Camera*)pCurrentScene->getComponent(cameraID, ComponentType::PK_CAMERA);
        if (!pCamera)
        {
            Debug::log(
                "@SkinnedRenderer::render "
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
        std::unordered_map<PK_id, int>::iterator batchIt;
        for (batchIt = _identifierBatchMapping.begin(); batchIt != _identifierBatchMapping.end(); ++batchIt)
        {
            PK_id batchIdentifier = batchIt->first;
            SkinnedMeshBatch* pBatch = _batches[batchIt->second];
            if (_batchMeshMapping.find(batchIdentifier) == _batchMeshMapping.end())
                _batchMeshMapping[batchIt->first] = (Mesh*)resManager.getResource(batchIdentifier);

            if (_batchModelMapping.find(_batchModelIDMapping[batchIdentifier]) == _batchModelMapping.end())
                _batchModelMapping[_batchModelIDMapping[batchIdentifier]] = (Model*)resManager.getResource(_batchModelIDMapping[batchIdentifier]);

            Mesh* pMesh = _batchMeshMapping[batchIdentifier];
            Model* pModel = _batchModelMapping[_batchModelIDMapping[batchIdentifier]];
            if (!pMesh)
            {
                Debug::log(
                    "@SkinnedRenderer::render "
                    "Failed to find batch's mesh from resource manager with "
                    "batch identifier: " + std::to_string(batchIdentifier),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                continue;
            }
            if (!pModel)
            {
                Debug::log(
                    "@SkinnedRenderer::render "
                    "Failed to find batch's model from resource manager with "
                    "batch identifier: " + std::to_string(batchIdentifier),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                continue;
            }

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

            std::vector<Buffer*> vertexBuffers = { pVertexBuffer };
            pRenderCmd->bindVertexBuffers(
                pCurrentCmdBuf,
                0, 1,
                vertexBuffers
            );

            // Get "common descriptor set" from master renderer
            MasterRenderer& masterRenderer = pApp->getMasterRenderer();
            // Update material properties ubo
            const vec4& materialProperties = pBatch->materialProperties;
            _materialPropsUniformBuffers[0]->update(&materialProperties, sizeof(vec4));

            for (int i = 0; i < pBatch->occupiedCount; ++i)
            {
                const entityID_t rootJoint = pBatch->rootJoints[i];

                // Update joint matrices buffer to match this renderable's skeleton
                std::vector<mat4> jointMatrices(s_maxJoints);
                const Pose& bindPose = pModel->accessBindPose();

                update_joint_matrices(
                    pCurrentScene,
                    rootJoint,
                    0,
                    jointMatrices,
                    bindPose
                );

                _jointMatricesBuffer[0]->update(
                    jointMatrices.data(),
                    jointMatrices.size() * sizeof(mat4)
                );

                // NOTE: Not sure if its allowed in vulkan to call cmdBindDescriptorSets
                // multiple times like we do here!
                std::vector<const DescriptorSet*> toBind =
                {
                    masterRenderer.getCommonDescriptorSet(),
                    masterRenderer.getEnvironmentDescriptorSet(),
                    masterRenderer.getDirectionalLightDescriptorSet(),
                    _jointDescriptorSet[0],
                    pBatch->materialDescriptorSet[0]
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
                    1,
                    0,
                    0,
                    0
                );
            }
        }

        pRenderCmd->endCmdBuffer(pCurrentCmdBuf);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            Debug::log("___TEST___SKINNED RENDERER GL ERR: " + opengl::gl_error_to_string(err));
    }

    void SkinnedRenderer::flush()
    {
        _identifierBatchMapping.clear();
    }

    void SkinnedRenderer::freeDescriptorSets()
    {
        for (DescriptorSet* pJointDescriptorSet : _jointDescriptorSet)
            delete pJointDescriptorSet;
        _jointDescriptorSet.clear();
        for (Buffer* pJointUniformBuffer : _jointMatricesBuffer)
            delete pJointUniformBuffer;
        _jointMatricesBuffer.clear();

        _identifierBatchMapping.clear();
        for (SkinnedMeshBatch* pBatch : _batches)
            delete pBatch;
        _batches.clear();
        _batchMeshMapping.clear();

        _batchModelIDMapping.clear();
        _batchMeshMapping.clear();
    }

    void SkinnedRenderer::createJointUniformBuffers(size_t swapchainImageCount)
    {
        std::vector<mat4> initialMatrices(s_maxJoints);
        _jointMatricesBuffer.resize(swapchainImageCount);
        memset(initialMatrices.data(), 0, sizeof(mat4) * s_maxJoints);
        for (int i = 0; i < swapchainImageCount; ++i)
        {
            _jointMatricesBuffer[i] = Buffer::create(
                initialMatrices.data(),
                sizeof(mat4),
                initialMatrices.size(),
                BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                true
            );
        }
    }

    void SkinnedRenderer::createJointDescriptorSets(const std::vector<Buffer*>& jointBuffers)
    {
        _jointDescriptorSet.resize(jointBuffers.size());
        for (int i = 0; i < jointBuffers.size(); ++i)
        {
            _jointDescriptorSet[i] = new DescriptorSet(
                _jointDescriptorSetLayout,
                1,
                { jointBuffers[i] }
            );
        }
    }
}
