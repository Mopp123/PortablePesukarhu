#include "ModelLoading.h"
#include "core/Debug.h"
#include "graphics/Buffers.h"
#include "graphics/platform/opengl/OpenglContext.h"
#include "graphics/animation/Pose.h"
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "tiny_gltf.h"


#define GLTF_POINTS         0x0
#define GLTF_LINES          0x0001
#define GLTF_LINE_LOOP      0x0002
#define GLTF_LINE_STRIP     0x0003
#define GLTF_TRIANGLES      0x0004
#define GLTF_TRIANGLE_STRIP 0x0005
#define GLTF_TRIANGLE_FAN   0x0006
#define GLTF_QUADS          0x0007
#define GLTF_QUAD_STRIP     0x0008


namespace pk
{
    struct GLTFBufferData
    {
        PK_byte* pData = nullptr;
        size_t size = 0;
        BufferUsageFlagBits usage = BufferUsageFlagBits::BUFFER_USAGE_NONE;

        GLTFBufferData(
            PK_byte* data,
            size_t size,
            BufferUsageFlagBits usage
        ) :
            size(size),
            usage(usage)
        {
            pData = new PK_byte[size];
            memset(pData, 0, size);
            memcpy(pData, data, size);
        }

        GLTFBufferData(const GLTFBufferData& other) :
            size(other.size),
            usage(other.usage)
        {
            pData = new PK_byte[size];
            memcpy(pData, other.pData, size);
        }

        ~GLTFBufferData()
        {
            delete[] pData;
        }
    };


    static BufferUsageFlagBits gltf_buf_view_target_to_engine(int gltfTarget)
    {
        // values taken from: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_bufferview_target
        // should be safe to use..
        // 34962 = ARRAY_BUFFER
        // 34963 = ELEMENT_ARRAY_BUFFER
        switch (gltfTarget)
        {
            // ARRAY_BUFFER
            case (34962) :
                return BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT;
            // ELEMENT_ARRAY_BUFFER
            case (34963) :
                return BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT;
            default :
            {
                Debug::log(
                    "@gltf_buf_view_target_to_engine "
                    "invalid buffer view target: " + std::to_string(gltfTarget),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return BufferUsageFlagBits::BUFFER_USAGE_NONE;
            }
        }
    }


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
        // From opengl spec:
        // float = 0x1406
        // int = 0x1404
        const uint64_t gltfInt = 0x1404;
        const uint64_t gltfFloat = 0x1406;

        // float types
        if (gltfAccessorComponentType == gltfFloat)
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
                        "invalid accessor component type: " + std::to_string(gltfAccessorComponentType),
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return ShaderDataType::None;

            }
        }
        // int types
        else if (gltfAccessorComponentType == gltfInt)
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
            "Unexpected input",
            Debug::MessageType::PK_FATAL_ERROR
        );
        return ShaderDataType::None;
    }


    static Mesh* load_mesh_gltf(
        tinygltf::Model& gltfModel,
        tinygltf::Mesh& gltfMesh,
        const std::vector<GLTFBufferData>& gltfBuffers,
        bool saveDataHostSide
    )
    {
        Mesh* pMesh = nullptr;
        size_t combinedVertexBufferSize = 0;
        size_t indexBufferIndex = 0;
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
        for (size_t i = 0; i < gltfMesh.primitives.size(); ++i)
        {
            tinygltf::Primitive primitive = gltfMesh.primitives[i];

            tinygltf::Accessor indexAccessor = gltfModel.accessors[primitive.indices];
            indexBufferIndex =  indexAccessor.bufferView;
            indexBufferLength = indexAccessor.count;
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
                int attribLocation = -1;
                if (attrib.first.compare("POSITION") == 0) attribLocation = 0;
                if (attrib.first.compare("NORMAL") == 0) attribLocation = 1;
                if (attrib.first.compare("TEXCOORD_0") == 0) attribLocation = 2;

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
                    ShaderDataType shaderDataType = gltf_accessor_component_type_to_engine(
                        accessor.componentType,
                        componentCount
                    );

                    VertexBufferElement elem(attribLocation, shaderDataType);
                    int buffersIndex = accessor.bufferView;
                    sortedVertexBufferAttributes[attribLocation] = std::make_pair(elem, buffersIndex);

                    const GLTFBufferData& gltfBuffer = gltfBuffers[buffersIndex];
                    combinedVertexBufferSize += gltfBuffer.size;
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

        // TODO: Attempt to find the index buffer
        const GLTFBufferData& gltfIndexBuffer = gltfBuffers[indexBufferIndex];
        if (gltfIndexBuffer.usage != BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT)
        {
            Debug::log(
                "@load_mesh_gltf "
                "gltfMesh using index buffer at: " + std::to_string(indexBufferIndex) + " "
                "but this buffer had not been flagged with BUFFER_USAGE_INDEX_BUFFER_BIT",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }

        PK_byte* pCombinedRawBuffer = new PK_byte[combinedVertexBufferSize];
        const size_t attribCount = sortedVertexBufferAttributes.size();
        size_t srcOffsets[attribCount];
        memset(srcOffsets, 0, sizeof(size_t) * attribCount);
        size_t dstOffset = 0;
        std::vector<VertexBufferElement> vbLayoutElements(sortedVertexBufferAttributes.size());

        size_t currentAttribIndex = 0;
        while (dstOffset < combinedVertexBufferSize)
        {
            const std::pair<VertexBufferElement, int>& currentAttrib = sortedVertexBufferAttributes[currentAttribIndex];
            const size_t currentAttribElemSize = get_shader_data_type_size(currentAttrib.first.getType());

            const GLTFBufferData& srcBuffer = gltfBuffers[currentAttrib.second];
            PK_byte* srcData = srcBuffer.pData + srcOffsets[currentAttribIndex];
            memcpy(pCombinedRawBuffer + dstOffset, srcData, currentAttribElemSize);

            srcOffsets[currentAttribIndex] += currentAttribElemSize;
            dstOffset += currentAttribElemSize;
            // DANGER!!!!
            vbLayoutElements[currentAttrib.first.getLocation()] = currentAttrib.first; // DANGER!!!

            currentAttribIndex += 1;
            currentAttribIndex = currentAttribIndex % attribCount;
        }

        size_t indexBufElemSize = gltfIndexBuffer.size / indexBufferLength;
        Buffer* pIndexBuffer = Buffer::create(
            gltfIndexBuffer.pData,
            indexBufElemSize,
            indexBufferLength,
            gltfIndexBuffer.usage,
            false
        );

        Buffer* pVertexBuffer = Buffer::create(
            pCombinedRawBuffer,
            1,
            combinedVertexBufferSize,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT,
            false
        );

        Debug::log(
            "@load_mesh_gltf Creating mesh with vertex buffer layout:"
        );
        for (const VertexBufferElement& e : vbLayoutElements)
            Debug::log("\tlocation = " + std::to_string(e.getLocation()) + " type = " + std::to_string(e.getType()));

        pMesh = new Mesh(
            pVertexBuffer,
            pIndexBuffer,
            nullptr,
            { vbLayoutElements, VertexInputRate::VERTEX_INPUT_RATE_VERTEX }
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
        // NOTE: Not sure is gltf matrix row or col major..
        mat4 jointMatrix = to_engine_matrix(node.matrix);

        vec3 translation;
        quat rotation;
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
        }
        if (node.scale.size() == 3)
        {
            scale = vec3(
                node.scale[0],
                node.scale[1],
                node.scale[2]
            );
        }

        Joint joint = {
            translation,
            rotation,
            scale,
            jointMatrix
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
        for (const tinygltf::AnimationChannel& channel : channels)
        {
            const tinygltf::AnimationSampler& sampler = gltfAnim.samplers[channel.sampler];

            Debug::log(
                "___TEST___processing node: " + std::to_string(channel.target_node) + " "
                "path: " + channel.target_path + " "
                "at frame: " + std::to_string(keyframeIndex)
            );

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
                joint.translation = translationValue;
            }
            else if (channel.target_path == "rotation")
            {
                PK_byte* pAnimData = (PK_byte*)&animDataBuffer.data[animDataAccessor.byteOffset + bufferView.byteOffset + (sizeof(quat) * useKeyframe)];
                quat rotationValue = *((quat*)pAnimData);
                rotationValue = rotationValue.normalize();
                joint.rotation = rotationValue;
            }
        }
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
        // We require all nodes to have same amount of keyframes here!
        size_t maxKeyframes = 0;
        Debug::log("___TEST___Channel data:");
        for (tinygltf::AnimationChannel& channel : gltfAnim.channels)
        {
            const tinygltf::AnimationSampler& sampler = gltfAnim.samplers[channel.sampler];
            size_t keyframeCount = gltfModel.accessors[sampler.input].count;
            maxKeyframes = std::max(maxKeyframes, keyframeCount);

            Debug::log(
                "   sampler: " + std::to_string(channel.sampler) + "\n"
                "       input: " + std::to_string(sampler.input) + "\n"
                "   node: " + std::to_string(channel.target_node) + "\n"
                "   path: " + channel.target_path + "\n"
                "   keyframes: " + std::to_string(keyframeCount)
            );

            nodeChannelsMapping[channel.target_node].push_back(channel);
        }

        //std::vector<Joint> poseJoints(nodePoseJointMapping.size());
        std::unordered_map<int, std::vector<Joint>> poseJoints;
        for (int i = 0; i < maxKeyframes; ++i)
            poseJoints[i].resize(nodePoseJointMapping.size());

        std::unordered_map<int, std::vector<tinygltf::AnimationChannel>>::const_iterator ncIt;
        for (ncIt = nodeChannelsMapping.begin(); ncIt != nodeChannelsMapping.end(); ++ncIt)
        {
            // gltf appears to provide keyframes in reverse order -> take that in account!
            for (int keyframeIndex = maxKeyframes - 1; keyframeIndex >= 0; --keyframeIndex)
            {
                int poseJointIndex = nodePoseJointMapping[ncIt->first];
                Debug::log("___TEST___adding node: " + std::to_string(ncIt->first) + " to index: " + std::to_string(poseJointIndex));
                poseJoints[keyframeIndex][poseJointIndex] = create_anim_pose_joint(
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

        // Combine all gltf buffers into our type for easier parsing later..
        std::vector<GLTFBufferData> gltfBuffers;
        for (size_t i = 0; i < gltfModel.bufferViews.size(); ++i)
        {
            const tinygltf::BufferView &bufferView = gltfModel.bufferViews[i];
            // TODO: Take buffer views with target = 0 into account when loading
            // Materials and/or skinned models!
            if (bufferView.target == 0)
            {
                Debug::log(
                    "@load_model_gltf "
                    "bufferView target was 0",
                    Debug::MessageType::PK_WARNING
                );
                continue;
            }

            const tinygltf::Buffer &gltfBuffer = gltfModel.buffers[bufferView.buffer];
            BufferUsageFlagBits usage = gltf_buf_view_target_to_engine(bufferView.target);
            gltfBuffers.push_back(
                {
                    (PK_byte*)(&gltfBuffer.data.at(0) + bufferView.byteOffset),
                    bufferView.byteLength,
                    usage
                }
            );
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

        std::vector<Mesh*> meshes;
        for (size_t i = 0; i < gltfModel.meshes.size(); ++i)
        {
            Debug::log("Processing mesh at index: " + std::to_string(i));
            tinygltf::Mesh& gltfMesh = gltfModel.meshes[i];
            Mesh* pMesh = load_mesh_gltf(
                gltfModel,
                gltfMesh,
                gltfBuffers,
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
            // Load animations if found
            // NOTE: For now supporting just a single animation
            size_t animCount = gltfModel.animations.size();
            if (animCount == 1)
            {
                animPoses = load_anim_poses(
                    gltfModel,
                    bindPose,
                    nodeJointMapping
                );
            }
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
            return new Model(meshes, bindPose, animPoses);
        else
            return new Model(meshes);
    }
}
