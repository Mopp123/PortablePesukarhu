#include "ModelLoading.h"
#include "core/Debug.h"
#include "graphics/Buffers.h"
#include "graphics/platform/opengl/OpenglContext.h"
#include <string>
#include <unordered_map>

#include <iostream>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "tiny_gltf.h"


#define GLTF_POINTS 0x0
#define GLTF_LINES 0x0001
#define GLTF_LINE_LOOP 0x0002
#define GLTF_LINE_STRIP 0x0003
#define GLTF_TRIANGLES 0x0004
#define GLTF_TRIANGLE_STRIP 0x0005
#define GLTF_TRIANGLE_FAN 0x0006
#define GLTF_QUADS 0x0007
#define GLTF_QUAD_STRIP 0x0008


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


    Model* load_model_glb(const std::string& filepath)
    {
        tinygltf::Model gltfModel;
        tinygltf::TinyGLTF loader;
        std::string error;
        std::string warning;

        //bool ret = loader.LoadBinaryFromFile(&gltfModel, &error, &warning, filepath); // for binary glTF(.glb)
        bool ret = loader.LoadASCIIFromFile(&gltfModel, &error, &warning, filepath);

        if (!warning.empty()) {
            Debug::log(
                "@load_model_glb "
                "Warning while loading .glb file: " + filepath + " "
                "tinygltf warning: " + warning,
                Debug::MessageType::PK_WARNING
            );
        }

        if (!error.empty()) {
            Debug::log(
                "@load_model_glb "
                "Error while loading .glb file: " + filepath + " "
                "tinygltf error: " + error,
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }

        if (!ret) {
            Debug::log(
                "@load_model_glb "
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
                    "@load_model_glb "
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
            "@load_model_glb "
            "Processing glTF Model with total scene count = " + std::to_string(gltfModel.scenes.size()) + " "
            "total node count = " + std::to_string(modelNodes.size())
        );

        // NOTE: Atm not supporting multiple scenes!
        if (gltfModel.scenes.size() != 1)
        {
            Debug::log(
                "@load_model_glb Multiple scenes not supported!",
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

        Model* pModel = new Model(meshes);
        return pModel;
    }
}
