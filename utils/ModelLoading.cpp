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
        //34962  = ARRAY_BUFFER
        //34963  = ELEMENT_ARRAY_BUFFER
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

    // TODO: get actual GLEnum values ftom specification to here!
    // Cannot use GLEnums cuz if not using opengl api, these are not defined!
    // -> those can be found from glew.h
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

    // NOTE: Possible issues:
    //  * Not sure if all vertex data is contiguous with the way this currently works:
    //  https://stackoverflow.com/questions/69303430/are-the-position-normals-texcoords-and-other-such-attributes-of-a-gltf-3d-mode
    //
    // Currently expected to push all stuff into a single buffer!
    // gltfModel refers to the "top node" <- Not 100% sure tho..
    // gltfMesh refers to single mesh in model
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

        // second = index in gltfBuffers
        std::vector<std::pair<VertexBufferElement, int>> layoutElements;
        for (size_t i = 0; i < gltfMesh.primitives.size(); ++i)
        {
            tinygltf::Primitive primitive = gltfMesh.primitives[i];
            tinygltf::Accessor indexAccessor = gltfModel.accessors[primitive.indices];

            indexBufferIndex =  indexAccessor.bufferView;
            indexBufferLength = indexAccessor.count;

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


                if (attrib.first.compare("POSITION") == 0) Debug::log("___TEST___ATTRIB = POS");
                if (attrib.first.compare("NORMAL") == 0) Debug::log("___TEST___ATTRIB = NORMAL");
                if (attrib.first.compare("TEXCOORD_0") == 0) Debug::log("___TEST___ATTRIB = UV");

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
                    // DANGER HERE!
                    int buffersIndex = accessor.bufferView; // not actually sure if correct?
                    layoutElements.push_back(std::make_pair(elem, buffersIndex));

                    const GLTFBufferData& gltfBuffer = gltfBuffers[buffersIndex];
                    //size_t elemSize = get_shader_data_type_size(shaderDataType);
                    //combinedVertexBufferSize += elemSize * gltfBuffer.length;
                    //Debug::log("___TEST___ADDED NEW ATTRIB size = " + std::to_string(elemSize) + " gltfBuffer.length = " + std::to_string(gltfBuffer.length));

                    // at this point gltfBuffers' length is the total size in bytes..
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

        // TODO:
        // Create one fat vertex buffer from gltfBuffers using the found vb layout
        //
        // Fucking dumb shit here, but have to do cuz the way engine's meshes work atm..
        // 4 = pos, normal, uv, index buffer
        PK_byte* pCombinedRawBuffer = new PK_byte[combinedVertexBufferSize];
        const size_t attribCount = layoutElements.size();
        size_t srcOffsets[attribCount];
        memset(srcOffsets, 0, sizeof(size_t) * attribCount);
        size_t dstOffset = sizeof(float) * (3 + 2);
        std::vector<VertexBufferElement> vbLayoutElements(layoutElements.size());


        int firstIndex = 0;
        for (const std::pair<VertexBufferElement, int>& a : layoutElements)
        {
            if (a.first.getLocation() == 0)
                break;
            firstIndex++;
        }
        Debug::log("___TEST___FIRST INDEX = " + std::to_string(firstIndex));
        size_t currentAttribIndex = firstIndex;
        // THIS IS FUCKED ATM!!
        // * part of the reason is that first layout element is location=1 normal instead of location=0 position!
        // TODO: Make it actually do what u intended!
        while (dstOffset < combinedVertexBufferSize)
        {
            const std::pair<VertexBufferElement, int>& currentAttrib = layoutElements[currentAttribIndex];
            const size_t attribComponents = get_shader_data_type_component_count(currentAttrib.first.getType());
            const size_t currentAttribElemSize = get_shader_data_type_size(currentAttrib.first.getType());
            for (size_t i = 0; i < attribComponents; ++i)
            {
                const GLTFBufferData& srcBuffer = gltfBuffers[currentAttrib.second];
                PK_byte* srcData = srcBuffer.pData + srcOffsets[currentAttribIndex];
                memcpy(pCombinedRawBuffer + dstOffset, srcData, currentAttribElemSize);

                srcOffsets[currentAttribIndex] += currentAttribElemSize;
                dstOffset += currentAttribElemSize;
                // DANGER!!!!
                vbLayoutElements[currentAttrib.first.getLocation()] = currentAttrib.first; // DANGER!!!
            }
            currentAttribIndex += 1;
            currentAttribIndex = currentAttribIndex % attribCount;

            /*
            for (size_t i = 0; i < attribCount; ++i)
            {
                const std::pair<VertexBufferElement, int>& attrib = layoutElements[i];
                const GLTFBufferData& gltfBuffer = gltfBuffers[attrib.second];
                size_t elemSize = get_shader_data_type_size(attrib.first.getType());
                if (srcOffsets[i] + elemSize > gltfBuffer.size)
                {
                    Debug::log(
                        "@load_mesh_gltf "
                        "Error while creating combined vertex buffer(1)\n"
                        "\tbuf size = " + std::to_string(gltfBuffer.size) + "\n"
                        "\telem size = " + std::to_string(elemSize) + "\n"
                        "\tcurrent src offset = " + std::to_string(srcOffsets[i]) + "\n"
                        "\tdst offset = " + std::to_string(dstOffset),
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return nullptr;
                }

                PK_byte* srcData = gltfBuffer.pData + srcOffsets[i];
                memcpy(pCombinedRawBuffer + dstOffset, srcData, elemSize);
                srcOffsets[i] += elemSize;
                dstOffset += elemSize;
                vbLayoutElements[i] = attrib.first;
            }
            */
        }

        size_t indexBufElemSize = gltfIndexBuffer.size / indexBufferLength;
        Buffer* pIndexBuffer = Buffer::create(
            gltfIndexBuffer.pData,
            indexBufElemSize,
            indexBufferLength,
            gltfIndexBuffer.usage,
            true
        );

        Buffer* pVertexBuffer = Buffer::create(
            pCombinedRawBuffer,
            1,
            combinedVertexBufferSize,
            BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT,
            true
        );

        Debug::log("___TEST___creating model with vb layout:");
        for (const VertexBufferElement& e : vbLayoutElements)
        {
            Debug::log("\tlocation = " + std::to_string(e.getLocation()) + " type = " + std::to_string(e.getType()));
        }


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
            if (bufferView.target == 0)
            {
                Debug::log(
                    "@load_model_glb "
                    "bufferView target was 0",
                    Debug::MessageType::PK_WARNING
                );
                continue;  // Unsupported bufferView.
                /*
                   From spec2.0 readme:
                    https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
                    ... drawArrays function should be used with a count equal to
                    the count            property of any of the accessors referenced by the
                    attributes            property            (they are all equal for a given
                    primitive).
                */
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
        //  1. Make sure node has mesh!
        //  2. Support multiple meshes
        const tinygltf::Scene &gltfScene = gltfModel.scenes[gltfModel.defaultScene];
        tinygltf::Node& node = gltfModel.nodes[gltfScene.nodes[0]];
        tinygltf::Mesh& gltfMesh = gltfModel.meshes[node.mesh];
        Mesh* pMesh = load_mesh_gltf(
            gltfModel,
            gltfMesh,
            gltfBuffers,
            true
        );
        Model* pModel = new Model({ pMesh });

        return pModel;
    }
}
