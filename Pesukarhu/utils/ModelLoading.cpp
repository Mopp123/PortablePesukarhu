#include "ModelLoading.h"
#include "Pesukarhu/core/Debug.h"
#include "Pesukarhu/graphics/Buffers.h"
#include "Pesukarhu/graphics/platform/opengl/OpenglContext.h"
#include "Pesukarhu/graphics/animation/Pose.h"
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "Pesukarhu/tiny_gltf.h"

#define GLTF_POINTS         0x0
#define GLTF_LINES          0x0001
#define GLTF_LINE_LOOP      0x0002
#define GLTF_LINE_STRIP     0x0003
#define GLTF_TRIANGLES      0x0004
#define GLTF_TRIANGLE_STRIP 0x0005
#define GLTF_TRIANGLE_FAN   0x0006
#define GLTF_QUADS          0x0007
#define GLTF_QUAD_STRIP     0x0008

// Same as opengl types.. GL_INT, etc..
#define GLTF_ACCESSOR_COMPONENT_TYPE_INT 0x1404
#define GLTF_ACCESSOR_COMPONENT_TYPE_FLOAT 0x1406
#define GLTF_ACCESSOR_COMPONENT_TYPE_UNSIGNED_BYTE 0x1401


namespace pk
{
    static std::string gltf_primitive_mode_to_string(int mode)
    {
        switch (mode)
        {
            case GLTF_POINTS :
                return "GLTF_POINTS";
            case GLTF_LINES :
                return "GLTF_LINES";
            case GLTF_LINE_LOOP :
                return "GLTF_LINE_LOOP";
            case GLTF_LINE_STRIP :
                return "GLTF_LINE_STRIP";
            case GLTF_TRIANGLES :
                return "GLTF_TRIANGLES";
            case GLTF_TRIANGLE_STRIP :
                return "GLTF_TRIANGLE_STRIP";
            case GLTF_TRIANGLE_FAN :
                return "GLTF_TRIANGLE_FAN";
            case GLTF_QUADS :
                return "GLTF_QUADS";
            case GLTF_QUAD_STRIP :
                return "GLTF_QUAD_STRIP";
            default:
                return "none";
        }
    }


    // TODO: get actual GLEnum values ftom specification to here!
    // Cannot use GLenums cuz if not using opengl api, these are not defined!
    static ShaderDataType gltf_accessor_component_type_to_engine(
        int gltfAccessorComponentType,
        int gltfComponentCount
    )
    {
        // float types
        if (gltfAccessorComponentType == GLTF_ACCESSOR_COMPONENT_TYPE_FLOAT)
        {
            switch (gltfComponentCount)
            {
                case (1) :
                    return ShaderDataType::Float;
                case (2) :
                    return ShaderDataType::Float2;
                case (3) :
                    return ShaderDataType::Float3;
                case (4) :
                    return ShaderDataType::Float4;
                default:
                    Debug::log(
                        "@gltf_accessor_component_type_to_engine(float) "
                        "invalid accessor component type: " + std::to_string(gltfAccessorComponentType) + " "
                        "Component count: " + std::to_string(gltfComponentCount),
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return ShaderDataType::None;

            }
        }
        // int types
        else if (gltfAccessorComponentType == GLTF_ACCESSOR_COMPONENT_TYPE_INT)
        {
            switch (gltfComponentCount)
            {
                case (1) :
                    return ShaderDataType::Int;
                case (2) :
                    return ShaderDataType::Int2;
                case (3) :
                    return ShaderDataType::Int3;
                case (4) :
                    return ShaderDataType::Int4;
                default:
                    Debug::log(
                        "@gltf_accessor_component_type_to_engine(int) "
                        "invalid accessor component type: " + std::to_string(gltfAccessorComponentType),
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return ShaderDataType::None;
            }
        }
        Debug::log(
            "@gltf_accessor_component_type_to_engine "
            "Unexpected input! "
            "gltf accessor component type: " + std::to_string(gltfAccessorComponentType) + " "
            "Component count:  " + std::to_string(gltfComponentCount),
            Debug::MessageType::PK_FATAL_ERROR
        );
        return ShaderDataType::None;
    }


    // NOTE:
    // Current limitations!
    // * Expecting to have only a single set of primitives for single mesh!
    //  -> if thats not the case shit gets fucked!
    static Mesh* load_mesh_gltf(
        tinygltf::Model& gltfModel,
        tinygltf::Mesh& gltfMesh,
        bool saveDataHostSide
    )
    {
        Mesh* pMesh = nullptr;
        size_t combinedVertexBufferSize = 0;
        tinygltf::Accessor* pIndexBufferAccessor = nullptr;
        size_t indexBufferLength = 0;

        // sortedVertexBufferAttributes is used to create single vertex buffer containing
        // vertex positions, normals and uv coords in that order starting from location = 0.
        // This is also for creating VertexBufferLayout in correct order.
        //
        // key: attrib location
        // value:
        //  first: pretty obvious...
        //  second: index in gltfBuffers
        std::unordered_map<int, std::pair<VertexBufferElement, int>> sortedVertexBufferAttributes;
        std::unordered_map<int, tinygltf::Accessor> attribAccessorMapping;
        std::unordered_map<int, size_t> actualAttribElemSize;
        for (size_t i = 0; i < gltfMesh.primitives.size(); ++i)
        {
            tinygltf::Primitive primitive = gltfMesh.primitives[i];

            if (!pIndexBufferAccessor)
            {
                pIndexBufferAccessor = &gltfModel.accessors[primitive.indices];
                indexBufferLength = pIndexBufferAccessor->count;
            }

            if (indexBufferLength <= 0)
            {
                Debug::log(
                    "@load_mesh_gltf "
                    "Index buffer length was 0",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }

            for (auto &attrib : primitive.attributes)
            {
                tinygltf::Accessor accessor = gltfModel.accessors[attrib.second];

                int componentCount = 1;
                if (accessor.type != TINYGLTF_TYPE_SCALAR)
                    componentCount = accessor.type;

                // NOTE: using this expects shader having vertex attribs in same order
                // as here starting from pos 0
                int attribLocation = -1; // NOTE: attribLocation is more like attrib index here!
                if (attrib.first == "POSITION")     attribLocation = 0;
                if (attrib.first == "NORMAL")       attribLocation = 1;
                if (attrib.first == "TEXCOORD_0")   attribLocation = 2;

                if (attrib.first == "WEIGHTS_0")    attribLocation = 3;
                if (attrib.first == "JOINTS_0")     attribLocation = 4;

                if (attribLocation > -1)
                {
                    // NOTE: Possible issue if file format expects some
                    // shit to be normalized on the application side..
                    int normalized = accessor.normalized;
                    if (normalized)
                    {
                        Debug::log(
                            "@load_mesh_gltf "
                            "Vertex attribute expected to be normalized from gltf file side "
                            "but engine doesnt currently support this!",
                            Debug::MessageType::PK_FATAL_ERROR
                        );
                    }

                    // Joint indices may come as unsigned bytes -> switch those to float for now..
                    ShaderDataType shaderDataType = ShaderDataType::None;
                    if (accessor.componentType != GLTF_ACCESSOR_COMPONENT_TYPE_UNSIGNED_BYTE)
                    {
                        shaderDataType = gltf_accessor_component_type_to_engine(accessor.componentType, componentCount);
                    }
                    else
                    {
                        shaderDataType = gltf_accessor_component_type_to_engine(
                            TINYGLTF_COMPONENT_TYPE_FLOAT,
                            componentCount
                        );
                        actualAttribElemSize[attribLocation] = 1 * 4; // 1 byte for each vec4 component
                    }
                    size_t attribSize = get_shader_data_type_size(shaderDataType);

                    int bufferViewIndex = accessor.bufferView;
                    VertexBufferElement elem(attribLocation, shaderDataType);
                    sortedVertexBufferAttributes[attribLocation] = std::make_pair(elem, bufferViewIndex);
                    attribAccessorMapping[attribLocation] = accessor;

                    size_t elemCount = accessor.count;
                    combinedVertexBufferSize += elemCount * attribSize;
                }
                else
                {
                    Debug::log(
                        "@load_mesh_gltf "
                        "Vertex attribute location was missing",
                        Debug::MessageType::PK_WARNING
                    );
                    //std::cout << "vaa missing: " << attrib.first << std::endl;
                }
            }
        }

        if (!pIndexBufferAccessor)
        {
            Debug::log(
                "@load_mesh_gltf "
                "Failed to find index buffer from gltf model! Currently index buffer is required for all meshes!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        const tinygltf::BufferView& indexBufferView = gltfModel.bufferViews[pIndexBufferAccessor->bufferView];
        const size_t indexBufElemSize = indexBufferView.byteLength / pIndexBufferAccessor->count;
        void* pIndexBufferData = gltfModel.buffers[indexBufferView.buffer].data.data() + indexBufferView.byteOffset + pIndexBufferAccessor->byteOffset;
        Buffer* pIndexBuffer = Buffer::create(
            pIndexBufferData,
            indexBufElemSize,
            indexBufferLength,
            BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT,
            BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STATIC,
            false
        );

        PK_byte* pCombinedRawBuffer = new PK_byte[combinedVertexBufferSize];
        const size_t attribCount = sortedVertexBufferAttributes.size();
        size_t srcOffsets[attribCount];
        memset(srcOffsets, 0, sizeof(size_t) * attribCount);
        size_t dstOffset = 0;
        std::vector<VertexBufferElement> vbLayoutElements(sortedVertexBufferAttributes.size());

        size_t currentAttribIndex = 0;


        // TESTING
        const size_t stride = sizeof(float) * 3 * 2 + sizeof(float) * 2 + sizeof(float) * 4 * 2;
        const size_t vertexCount = combinedVertexBufferSize / stride;
        std::unordered_map<int, std::pair<vec4, vec4>> vertexJointData;
        int vertexIndex = 0;
        int i = 0;

        while (dstOffset < combinedVertexBufferSize)
        {
            const std::pair<VertexBufferElement, int>& currentAttrib = sortedVertexBufferAttributes[currentAttribIndex];
            size_t currentAttribElemSize = get_shader_data_type_size(currentAttrib.first.getType());

            size_t gltfInternalSize = currentAttribElemSize;
            if (actualAttribElemSize.find(currentAttribIndex) != actualAttribElemSize.end())
                gltfInternalSize = actualAttribElemSize[currentAttribIndex];

            tinygltf::BufferView& bufView = gltfModel.bufferViews[currentAttrib.second];
            PK_ubyte* pSrcBuffer = (PK_ubyte*)(gltfModel.buffers[bufView.buffer].data.data() + bufView.byteOffset + attribAccessorMapping[currentAttribIndex].byteOffset + srcOffsets[currentAttribIndex]);

            // If attrib "gltf internal type" wasn't float
            //  -> we need to convert it into that (atm done only for joint ids buf which are ubytes)
            if (currentAttrib.first.getLocation() == 4)
            {
                vec4 val;
                val.x = (float)*pSrcBuffer;
                val.y = (float)*(pSrcBuffer + 1);
                val.z = (float)*(pSrcBuffer + 2);
                val.w = (float)*(pSrcBuffer + 3);
                vertexJointData[vertexIndex].second = val;

                memcpy(pCombinedRawBuffer + dstOffset, &val, sizeof(vec4));
            }
            else
            {
                // make all weights sum be 1
                if (currentAttrib.first.getLocation() == 3)
                {
                    vec4 val;
                    val.x = (float)*pSrcBuffer;
                    val.y = (float)*(pSrcBuffer + sizeof(float));
                    val.z = (float)*(pSrcBuffer + sizeof(float) * 2);
                    val.w = (float)*(pSrcBuffer + sizeof(float) * 3);
                    const float sum = val.x + val.y + val.z + val.w;
                    if (sum != 0.0f)
                    {
                        val.x /= sum;
                        val.y /= sum;
                        val.z /= sum;
                        val.w /= sum;
                    }
                    else if (sum != 1.0f)
                    {
                        val = vec4(1.0f, 0, 0, 0);
                    }
                    vertexJointData[vertexIndex].first = val;

                    memcpy(pCombinedRawBuffer + dstOffset, &val, sizeof(vec4));
                }
                else
                {
                    memcpy(pCombinedRawBuffer + dstOffset, pSrcBuffer, gltfInternalSize);
                }
            }


            srcOffsets[currentAttribIndex] += gltfInternalSize;
            dstOffset += currentAttribElemSize;
            // DANGER!!!!
            vbLayoutElements[currentAttrib.first.getLocation()] = currentAttrib.first; // DANGER!!!

            currentAttribIndex += 1;
            currentAttribIndex = currentAttribIndex % attribCount;

            ++i;
            if ((i % 5) == 0)
                ++vertexIndex;
        }

        Buffer* pVertexBuffer = Buffer::create(
            pCombinedRawBuffer,
            1,
            combinedVertexBufferSize,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT,
            BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STATIC,
            false
        );

        Debug::log(
            "@load_mesh_gltf Creating mesh with vertex buffer layout:"
        );
        for (const VertexBufferElement& e : vbLayoutElements)
            Debug::log("\tlocation = " + std::to_string(e.getLocation()) + " type = " + std::to_string(e.getType()));

        pMesh = new Mesh(
            { pVertexBuffer },
            pIndexBuffer,
            nullptr
        );
        return pMesh;
    }


    static mat4 to_engine_matrix(const std::vector<double>& gltfMatrix)
    {
        // Defaulting to 0 matrix to be able to test does this have matrix at all!
        mat4 matrix(0.0f);
        if (gltfMatrix.size() == 16)
        {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                    matrix[i + j * 4] = gltfMatrix[i + j * 4];
            }
        }
        return matrix;
    }

    static void add_joint(
        const tinygltf::Model& gltfModel,
        Pose& pose,
        int parentJointPoseIndex, // index to pose struct's parent joint. NOT glTF node index!
        int jointNodeIndex,
        std::unordered_map<int, int>& outNodeJointMapping
    )
    {
        const tinygltf::Node& node = gltfModel.nodes[jointNodeIndex];
        mat4 jointMatrix = to_engine_matrix(node.matrix);

        vec3 translation;
        quat rotation(0, 0, 0, 1);
        vec3 scale(1.0f, 1.0f, 1.0f);
        if (node.translation.size() == 3)
        {
            translation = vec3(
                node.translation[0],
                node.translation[1],
                node.translation[2]
            );
        }
        if (node.rotation.size() == 4)
        {
            rotation = quat(
                node.rotation[0],
                node.rotation[1],
                node.rotation[2],
                node.rotation[3]
            );
            rotation = rotation.normalize();
        }
        if (node.scale.size() == 3)
        {
            scale = vec3(
                node.scale[0],
                node.scale[1],
                node.scale[2]
            );
        }

        if (jointMatrix == mat4(0.0f))
        {
            mat4 translationMatrix(1.0f);
            translationMatrix[0 + 3 * 4] = translation.x;
            translationMatrix[1 + 3 * 4] = translation.y;
            translationMatrix[2 + 3 * 4] = translation.z;

            mat4 rotationMatrix = rotation.toRotationMatrix();

            mat4 scaleMatrix(1.0f);
            translationMatrix[0 + 0 * 4] = scale.x;
            translationMatrix[1 + 1 * 4] = scale.y;
            translationMatrix[2 + 2 * 4] = scale.z;

            jointMatrix = translationMatrix * rotationMatrix * scaleMatrix;
        }
        mat4 inverseBindMatrix = jointMatrix;
        inverseBindMatrix.inverse();

        Joint joint = {
            translation,
            rotation,
            scale,
            jointMatrix,
            inverseBindMatrix
        };
        const std::vector<int>& children = node.children;
        pose.joints.push_back(joint);
        pose.jointChildMapping.push_back({});
        const int currentJointPoseIndex = pose.joints.size() - 1;
        outNodeJointMapping[jointNodeIndex] = currentJointPoseIndex;
        if (parentJointPoseIndex != -1)
            pose.jointChildMapping[parentJointPoseIndex].push_back(currentJointPoseIndex);

        for (int childNodeIndex : children)
        {
            add_joint(
                gltfModel,
                pose,
                currentJointPoseIndex,
                childNodeIndex,
                outNodeJointMapping
            );
        }
    }


    static Joint create_anim_pose_joint(
        const tinygltf::Model& gltfModel,
        const tinygltf::Animation& gltfAnim,
        const std::vector<tinygltf::AnimationChannel>& channels,
        int keyframeIndex
    )
    {
        Joint joint;
        mat4 jointTranslationMatrix(1.0f);
        mat4 jointRotationMatrix(1.0f);
        mat4 jointScaleMatrix(1.0f);
        for (const tinygltf::AnimationChannel& channel : channels)
        {
            const tinygltf::AnimationSampler& sampler = gltfAnim.samplers[channel.sampler];

            // If keyframe count less than asked index -> use last available
            size_t keyframeCount = gltfModel.accessors[sampler.input].count;
            int useKeyframe = keyframeIndex;
            if (keyframeCount <= keyframeIndex)
                useKeyframe = keyframeCount - 1;

            // Get keyframe timestamp
            int keyframeBufferIndex = gltfModel.bufferViews[gltfModel.accessors[sampler.input].bufferView].buffer;
            const tinygltf::Buffer& keyframeBuffer = gltfModel.buffers[keyframeBufferIndex];

            // Get anim data of this channel
            const tinygltf::Accessor& animDataAccessor = gltfModel.accessors[sampler.output];
            const tinygltf::BufferView& bufferView = gltfModel.bufferViews[animDataAccessor.bufferView];
            const tinygltf::Buffer& animDataBuffer = gltfModel.buffers[bufferView.buffer];

            // NOTE: Scale anim not implemented!!!
            if (channel.target_path == "translation")
            {
                PK_byte* pAnimData = (PK_byte*)&animDataBuffer.data[animDataAccessor.byteOffset + bufferView.byteOffset + (sizeof(vec3) * useKeyframe)];
                vec3 translationValue = *((vec3*)pAnimData);

                jointTranslationMatrix[0 + 3 * 4] = translationValue.x;
                jointTranslationMatrix[1 + 3 * 4] = translationValue.y;
                jointTranslationMatrix[2 + 3 * 4] = translationValue.z;

                joint.translation = translationValue;
            }
            else if (channel.target_path == "rotation")
            {
                PK_byte* pAnimData = (PK_byte*)&animDataBuffer.data[animDataAccessor.byteOffset + bufferView.byteOffset + (sizeof(quat) * useKeyframe)];
                quat rotationValue = *((quat*)pAnimData);
                // Found out that with some configuration u can export gltfs from
                // blender without animation sampling -> causing "null rotations" for some keyframes..
                // *propably applies to translations and scaling as well..
                if (rotationValue.length() <= 0.0f)
                {
                    Debug::log(
                        "@create_anim_pose_joint "
                        "Rotation for keyframe: " + std::to_string(useKeyframe) + " "
                        "was null! Check your gltf export settings!",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                }
                rotationValue = rotationValue.normalize();
                jointRotationMatrix = rotationValue.toRotationMatrix();
                joint.rotation = rotationValue;
            }
        }

        joint.matrix = jointTranslationMatrix * jointRotationMatrix;
        mat4 inverseMatrix = joint.matrix;
        inverseMatrix.inverse();
        joint.inverseMatrix = inverseMatrix;

        return joint;
    }


    static std::vector<Pose> load_anim_poses(
        tinygltf::Model& gltfModel,
        const Pose& bindPose,
        std::unordered_map<int, int> nodePoseJointMapping
    )
    {
        tinygltf::Animation& gltfAnim = gltfModel.animations[0];

        std::unordered_map<int, std::vector<tinygltf::AnimationChannel>> nodeChannelsMapping;
        // We require all nodes to have same amount of keyframes at same time here!
        size_t maxKeyframes = 0;
        for (tinygltf::AnimationChannel& channel : gltfAnim.channels)
        {
            const tinygltf::AnimationSampler& sampler = gltfAnim.samplers[channel.sampler];
            size_t keyframeCount = gltfModel.accessors[sampler.input].count;
            maxKeyframes = std::max(maxKeyframes, keyframeCount);
            nodeChannelsMapping[channel.target_node].push_back(channel);
        }

        std::unordered_map<int, std::vector<Joint>> poseJoints;
        for (int i = 0; i < maxKeyframes; ++i)
            poseJoints[i].resize(nodePoseJointMapping.size());

        std::unordered_map<int, std::vector<tinygltf::AnimationChannel>>::const_iterator ncIt;
        for (ncIt = nodeChannelsMapping.begin(); ncIt != nodeChannelsMapping.end(); ++ncIt)
        {
            for (int keyframeIndex = 0; keyframeIndex < maxKeyframes; ++keyframeIndex)
            {
                int poseJointIndex = nodePoseJointMapping[ncIt->first];
                // gltf appears to provide keyframes in reverse order -> thats why: maxKeyframes - (keyframeIndex + 1)
                poseJoints[maxKeyframes - (keyframeIndex + 1)][poseJointIndex] = create_anim_pose_joint(
                    gltfModel,
                    gltfAnim,
                    ncIt->second,
                    keyframeIndex
                );
            }
        }

        std::vector<Pose> outPoses;
        std::unordered_map<int, std::vector<Joint>>::const_iterator pjIt;
        for (pjIt = poseJoints.begin(); pjIt != poseJoints.end(); ++pjIt)
        {
            Pose pose;
            pose.joints = pjIt->second;
            pose.jointChildMapping = bindPose.jointChildMapping;
            outPoses.push_back(pose);
        }

        return outPoses;
    }


    // NOTE:
    // Current limitations:
    //  * single mesh only
    //      - also excluding any camera, light nodes, etc..
    //  * single skeleton only
    //  * no material loading

    // TODO:
    //  * get simple sample files working from Khronos repo
    //      -> something was wrong with RiggedFigure, having something to do
    //      with multiple vertex attribs overlapping?
    Model* load_model_gltf(const std::string& filepath)
    {
        tinygltf::Model gltfModel;
        tinygltf::TinyGLTF loader;
        std::string error;
        std::string warning;

        std::string ext = filepath.substr(filepath.find("."), filepath.size());
        bool ret = false;
        if (ext == ".glb")
            ret = loader.LoadBinaryFromFile(&gltfModel, &error, &warning, filepath); // for binary glTF(.glb)
        else if (ext == ".gltf")
            ret = loader.LoadASCIIFromFile(&gltfModel, &error, &warning, filepath);

        if (!warning.empty()) {
            Debug::log(
                "@load_model_gltf "
                "Warning while loading .glb file: " + filepath + " "
                "tinygltf warning: " + warning,
                Debug::MessageType::PK_WARNING
            );
        }

        if (!error.empty()) {
            Debug::log(
                "@load_model_gltf "
                "Error while loading .glb file: " + filepath + " "
                "tinygltf error: " + error,
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }

        if (!ret) {
            Debug::log(
                "@load_model_gltf "
                "Error while loading .glb file: " + filepath + " "
                "Failed to parse file ",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }

        // TODO:
        //  * Support multiple meshes
        const tinygltf::Scene& gltfScene = gltfModel.scenes[gltfModel.defaultScene];
        const std::vector<tinygltf::Node>& modelNodes = gltfModel.nodes;

        Debug::log(
            "@load_model_gltf "
            "Processing glTF Model with total scene count = " + std::to_string(gltfModel.scenes.size()) + " "
            "total node count = " + std::to_string(modelNodes.size())
        );

        // NOTE: Atm not supporting multiple scenes!
        if (gltfModel.scenes.size() != 1)
        {
            Debug::log(
                "@load_model_gltf Multiple scenes not supported!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }

        // Load meshes
        std::vector<Mesh*> meshes;
        for (size_t i = 0; i < gltfModel.meshes.size(); ++i)
        {
            tinygltf::Mesh& gltfMesh = gltfModel.meshes[i];
            Mesh* pMesh = load_mesh_gltf(
                gltfModel,
                gltfMesh,
                true
            );
            if (pMesh)
                meshes.push_back(pMesh);
        }

        // Load skeleton if found
        size_t skinsCount = gltfModel.skins.size();
        Pose bindPose;
        std::vector<Pose> animPoses;
        if (skinsCount == 1)
        {
            int rootJointNodeIndex = gltfModel.skins[0].joints[0];
            // Mapping from gltf joint node index to our pose struct's joint index
            std::unordered_map<int, int> nodeJointMapping;
            add_joint(
                gltfModel,
                bindPose,
                -1, // index to pose struct's parent joint. NOT glTF node index!
                rootJointNodeIndex,
                nodeJointMapping
            );
            Debug::log("___TEST___loaded joints: " + std::to_string(nodeJointMapping.size()));


            // Load animations if found
            // NOTE: For now supporting just a single animation
            size_t animCount = gltfModel.animations.size();
            if (animCount > 1)
            {
                Debug::log(
                    "@load_model_gltf "
                    "Multiple animations(" + std::to_string(animCount) + ") "
                    "from file: " + filepath + " Currently only a single animation is supported",
                    Debug::MessageType::PK_FATAL_ERROR
                );
            }
            if (animCount == 1)
            {
                animPoses = load_anim_poses(
                    gltfModel,
                    bindPose,
                    nodeJointMapping
                );
            }

            // Load inverse bind matrices
            const tinygltf::Accessor& invBindAccess = gltfModel.accessors[gltfModel.skins[0].inverseBindMatrices];
            const tinygltf::BufferView& invBindBufView = gltfModel.bufferViews[invBindAccess.bufferView];
            const tinygltf::Buffer& invBindBuf = gltfModel.buffers[invBindBufView.buffer];
            size_t offset = invBindBufView.byteOffset + invBindAccess.byteOffset;
            for (int i = 0; i < bindPose.joints.size(); ++i)
            {
                mat4 inverseBindMatrix(1.0f);
                memcpy(&inverseBindMatrix, invBindBuf.data.data() + offset, sizeof(mat4));
                bindPose.joints[i].inverseMatrix = inverseBindMatrix;
                offset += sizeof(float) * 16;
            }
            // NOTE: Temporarely assign mesh's bind and animation poses here
            // TODO: Load bind pose an anim poses in same func where mesh loading happens!
            // TODO: Support multiple meshes
            meshes[0]->setBindPose(bindPose);
            meshes[0]->setAnimationPoses(animPoses);
        }
        else if (skinsCount > 1)
        {
            Debug::log(
                "@load_model_gltf "
                "Too many skins in file: " + std::to_string(skinsCount) + " "
                "Currently only 1 is supported",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }


        if (skinsCount == 1)
        {
            return new Model(meshes);
        }
        else
            return new Model(meshes);
    }
}
