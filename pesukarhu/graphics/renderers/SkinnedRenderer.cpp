#include "SkinnedRenderer.h"
#include <string>
#include "pesukarhu/core/Application.h"
#include "pesukarhu/ecs/components/renderable/SkinnedRenderable.h"
#include "pesukarhu/graphics/platform/opengl/OpenglContext.h"


namespace pk
{
    static int s_maxJoints = 50;

    // TODO: Consider some way to optimize
    // (this + transform system + animator system does almost the same shit
    //  -> like.. almost ^3 times the work needed.. necessary tho to have
    //  transforms behaving same way without any weird exceptions)
    static void update_joint_matrices(
        Scene* pScene,
        entityID_t jointEntity,
        int currentJointIndex,
        std::vector<mat4>& matrices,
        const Pose& bindPose,
        mat4 parentMatrix
    )
    {
        Transform* pTransform = (Transform*)pScene->getComponent(jointEntity, ComponentType::PK_TRANSFORM);

        const mat4& inverseBindMatrix = bindPose.joints[currentJointIndex].inverseMatrix;
        mat4 m(1.0f);
        if (currentJointIndex == 0)
            m = pTransform->accessTransformationMatrix();
        else
            m = parentMatrix * pTransform->accessLocalTransformationMatrix();

        matrices[currentJointIndex] = m * inverseBindMatrix;

        std::vector<entityID_t> childJointEntities = pScene->getChildren(jointEntity);
        for (int i = 0; i < bindPose.jointChildMapping[currentJointIndex].size(); ++i)
        {
            int childJointIndex = bindPose.jointChildMapping[currentJointIndex][i];
            update_joint_matrices(
                pScene,
                childJointEntities[i],
                childJointIndex,
                matrices,
                bindPose,
                m
            );
        }
    }

    static void update_joint_matrices_SPEED(
        const Scene* pScene,
        entityID_t jointEntity,
        int currentJointIndex,
        const Pose& bindPose,
        mat4 parentMatrix,
        void* pTargetBuffer
    )
    {
        Transform* pTransform = (Transform*)pScene->getComponent(jointEntity, ComponentType::PK_TRANSFORM);

        const mat4& inverseBindMatrix = bindPose.joints[currentJointIndex].inverseMatrix;

        mat4 m(1.0f);
        if (currentJointIndex == 0)
            m = pTransform->accessTransformationMatrix();
        else
            m = parentMatrix * pTransform->accessLocalTransformationMatrix();

        const mat4 m2 = m * inverseBindMatrix;

        const size_t targetOffset = currentJointIndex * sizeof(mat4);
        memcpy(((PK_byte*)pTargetBuffer) + targetOffset, &m2, sizeof(mat4));

        std::vector<entityID_t> childJointEntities = pScene->getChildren(jointEntity);
        for (int i = 0; i < bindPose.jointChildMapping[currentJointIndex].size(); ++i)
        {
            int childJointIndex = bindPose.jointChildMapping[currentJointIndex][i];
            update_joint_matrices_SPEED(
                pScene,
                childJointEntities[i],
                childJointIndex,
                bindPose,
                m,
                pTargetBuffer
            );
        }
    }

    static void update_joint_matrices_FASTEST(
        const Scene * const pScene,
        int currentJointIndex,
        const Pose& pose,
        void* pTargetBuffer
    )
    {
        const size_t targetOffset = currentJointIndex * sizeof(mat4);
        memcpy(
            ((PK_byte*)pTargetBuffer) + targetOffset,
            &pose.joints[currentJointIndex].matrix,
            sizeof(mat4)
        );
        //targetMatrices[currentJointIndex] = pose.joints[currentJointIndex].matrix;

        for (int i = 0; i < pose.jointChildMapping[currentJointIndex].size(); ++i)
        {
            update_joint_matrices_FASTEST(
                pScene,
                pose.jointChildMapping[currentJointIndex][i],
                pose,
                pTargetBuffer
            );
        }
    }


    // TODO: Make it possible on "client side project" to set initial
    // size on "init/scene switch stage"
    SkinnedMeshBatch::SkinnedMeshBatch(
        const Material* pMaterial,
        size_t initialCount,
        const DescriptorSetLayout& materialDescriptorSetLayout,
        // NOTE: using same ubo for all batches
        //  ->we only change the data of the ubo to match batch when we render the batch
        const std::vector<const Buffer*>& pMaterialPropsUniformBuffers
    ) :
        pMaterial(pMaterial),
        initialCount(initialCount)
    {
        jointMatrices.resize(initialCount * s_maxJoints);
        transformationMatrices.resize(initialCount);
        memset(jointMatrices.data(), 0, sizeof(mat4) * jointMatrices.size());

        const Texture* pDiffuseTexture = pMaterial->getDiffuseTexture(0);
        const Texture* pSpecularTexture = pMaterial->getSpecularTexture();

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

    void SkinnedMeshBatch::add(
        const Scene * const pScene,
        const Mesh * const  pMesh,
        const mat4& transformationMatrix,
        const AnimationData * const pAnimData
    )
    {
        // Not sure if this fucks up..
        if (occupiedCount * s_maxJoints >= jointMatrices.size())
        {
            jointMatrices.resize(jointMatrices.size() + s_maxJoints);
            Debug::log(
                "@SkinnedMeshBatch::add "
                "Insufficient space in jointMatrices. "
                "Resized to: " + std::to_string(jointMatrices.size() * sizeof(mat4)) + " bytes. "
                "Count: " + std::to_string(jointMatrices.size())
            );
        }
        if (occupiedCount >= transformationMatrices.size())
        {
            transformationMatrices.resize(transformationMatrices.size() + 1);
            Debug::log(
                "@SkinnedMeshBatch::add "
                "Insufficient space in transformationMatrices. "
                "Resized to: " + std::to_string(transformationMatrices.size() * sizeof(mat4)) + " bytes. "
                "Count: " + std::to_string(transformationMatrices.size())
            );
        }
        transformationMatrices[occupiedCount] = transformationMatrix;
        // where to put inputted skeleton in the "buffer"
        const size_t jointMatricesOffset = occupiedCount * (sizeof(mat4) * s_maxJoints);
        void* pTarget = ((PK_byte*)jointMatrices.data()) + jointMatricesOffset;
        /*
        update_joint_matrices_SPEED(
            pScene,
            rootJointEntity,
            0,
            pMesh->getBindPose(),
            mat4(1.0f),
            pTarget
        );
        */

        update_joint_matrices_FASTEST(
            pScene,
            0,
            pAnimData->getResultPose(),
            pTarget
        );
        ++occupiedCount;
    }

    // NOTE: Set joint matrices to 0 so no unnecessary resizing every frame
    void SkinnedMeshBatch::clear()
    {
        memset(
            jointMatrices.data(),
            0,
            sizeof(mat4) * jointMatrices.size()
        );
        memset(
            transformationMatrices.data(),
            0,
            sizeof(mat4) * transformationMatrices.size()
        );
        occupiedCount = 0;
    }


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
                    {{ 6, ShaderDataType::Mat4, s_maxJoints }} // locations 6 to 55
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
            {{ 57 }} // location 56 = transformationMatrix as push constant
        );
        // Specular texture
        DescriptorSetLayoutBinding specTextureDescSetLayoutBinding(
            1,
            1,
            DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {{ 58 }}
        );
        // "additional data"
        DescriptorSetLayoutBinding materialPropsDescSetLayoutBinding(
            2,
            2,
            DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT,
            {
                { 59, ShaderDataType::Float4 }, // color
                { 60, ShaderDataType::Float4 }  // "properties"
            }
        );


        _materialDescSetLayout = DescriptorSetLayout(
            {
                diffTextureDescSetLayoutBinding,
                specTextureDescSetLayoutBinding,
                materialPropsDescSetLayoutBinding
            }
        );

        // Create material "properties" ubo
        vec4 initialMaterialColor(1, 1, 1, 1);
        vec4 initialMaterialProps(1.0f, 32.0f, 0.0f, 0.0f);
        std::vector<PK_byte> initialMaterialData(sizeof(vec4) * 2);
        memcpy(initialMaterialData.data(), &initialMaterialColor, sizeof(vec4));
        memcpy(initialMaterialData.data() + sizeof(vec4), &initialMaterialProps, sizeof(vec4));
        for (int i = 0; i < swapchainImages; ++i)
        {
            Buffer* pMatUbo = Buffer::create(
                initialMaterialData.data(),
                sizeof(vec4) * 2,
                1,
                BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_DYNAMIC,
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
            false,
            0, // sending transformation matrices as push constants
            ShaderStageFlagBits::SHADER_STAGE_NONE
        );
    }

    void SkinnedRenderer::submit(
        const Component* const renderableComponent,
        const mat4& transformation,
        void* pCustomData,
        size_t customDataSize
    )
    {
        const Application* pApp = Application::get();
        const Scene* pScene = pApp->getCurrentScene();
        const ResourceManager& resManager = pApp->getResourceManager();

        const SkinnedRenderable * const pRenderable = (const SkinnedRenderable * const)renderableComponent;
        #ifdef PK_DEBUG_FULL
        if (!pCustomData)
        {
            Debug::log(
                "@SkinnedRenderer::submit "
                "Custom data was nullptr",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        #endif
        const AnimationData * const pAnimData = (const AnimationData * const)pCustomData;

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
            pBatch->add(pScene, pMesh, transformation, pAnimData);
        }
        else
        {
            Debug::log("___TEST___CREATING NEW BATCH! Current batches: " + std::to_string(_batches.size()));
            SkinnedMeshBatch* pNewBatch = new SkinnedMeshBatch(
                pMaterial,
                32,
                _materialDescSetLayout,
                _constMaterialPropsUniformBuffers
            );
            pNewBatch->add(pScene, pMesh, transformation, pAnimData);
            _batches.push_back(pNewBatch);
            _identifierBatchMapping[batchIdentifier] = _batches.size() - 1;
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

            Mesh* pMesh = _batchMeshMapping[batchIdentifier];
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
            const Material* pBatchMaterial = pBatch->pMaterial;
            vec4 materialData[2] = {
                pBatchMaterial->getColor(),
                {
                    pBatchMaterial->getSpecularStrength(),
                    pBatchMaterial->getSpecularShininess(),
                    (float)pBatchMaterial->isShadeless(),
                    0.0f
                }
            };
            _materialPropsUniformBuffers[0]->update(materialData, sizeof(vec4) * 2);

            for (int i = 0; i < pBatch->occupiedCount; ++i)
            {
                // Update joint matrices buffer to match this renderable's current pose
                const size_t entityJointsSize = sizeof(mat4) * s_maxJoints;
                const size_t jointMatricesOffset = i * entityJointsSize;
                void* pBatchJointData = ((PK_byte*)pBatch->jointMatrices.data()) + jointMatricesOffset;
                _jointMatricesBuffer[0]->update(
                    pBatchJointData,
                    entityJointsSize
                );

                // NOTE: Not sure if its allowed in vulkan to call cmdBindDescriptorSets
                // multiple times like we do here!
                //  -> UPDATE TO ABOVE!
                //      ->   our system doesnt allow that atm.. Vulkan propably does
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

                pRenderCmd->pushConstants(
                    pCurrentCmdBuf,
                    ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof(mat4),
                    &pBatch->transformationMatrices[i],
                    {
                        { 56, ShaderDataType::Mat4 }
                    }
                );

                pRenderCmd->drawIndexed(
                    pCurrentCmdBuf,
                    indexBufLen,
                    1,
                    0,
                    0,
                    0
                );
                // test draw same renderable multiple times..
                /*
                int testCount = 15 * 2 + 1;
                int spacing = 4.0f;
                mat4 originTransform = pBatch->transformationMatrices[i];
                for (int z = 0; z < testCount; ++z)
                {
                    for (int x = 0; x < testCount; ++x)
                    {
                        mat4 localTransform(1.0f);
                        localTransform[0 + 3 * 4] = x * spacing;
                        localTransform[1 + 3 * 4] = 0.0f;
                        localTransform[2 + 3 * 4] = z * spacing;

                        mat4 finalTransform = originTransform * localTransform;

                        pRenderCmd->pushConstants(
                            pCurrentCmdBuf,
                            ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT,
                            0,
                            sizeof(mat4),
                            &finalTransform,
                            {
                                { 56, ShaderDataType::Mat4 }
                            }
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
                */
            }
        }

        pRenderCmd->endCmdBuffer(pCurrentCmdBuf);
    }

    void SkinnedRenderer::flush()
    {
        for (SkinnedMeshBatch* pBatch : _batches)
            pBatch->clear();
        // No fucking idea why I did this here earlier
        //  -> causes creating new batches each frame without clearing prev batches
        //      -> huge leak!
        //_identifierBatchMapping.clear();
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
                BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_DYNAMIC,
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
