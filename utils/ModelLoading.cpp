#include "ModelLoading.h"
#include "core/Debug.h"
#include "graphics/platform/opengl/OpenglContext.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "tiny_gltf.h"


namespace pk
{
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
    static ShaderDataType gltf_accessor_component_type_to_engine(int gltfAccessorComponentType)
    {

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
        bool saveDataHostSide
    )
    {
        Mesh* pMesh = nullptr;

        //TODO:
        //  * Get vertex positions
        //  * Get normals

        // TODO: figure out how below works..
        for (size_t i = 0; i < gltfModel.bufferViews.size(); ++i)
        {
            const tinygltf::BufferView &bufferView = gltfModel.bufferViews[i];
            if (bufferView.target == 0)
            {
                Debug::log(
                    "@load_mesh_gltf "
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

            const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];

            // Create buffers
            BufferUsageFlagBits usage = gltf_buf_view_target_to_engine(bufferView.target);
            PK_byte* pRawBuffer = (PK_byte*)(&buffer.data.at(0) + bufferView.byteOffset);

            Buffer* pBuffer = Buffer::create(
                pRawBuffer,
                1,
                bufferView.byteLength,
                usage,
                saveDataHostSide
            );
            // Left below if u fucked up interpreting the example!
            /*
            glBindBuffer(bufferView.target, vbo);
            glBufferData(
                bufferView.target,
                bufferView.byteLength,
                &buffer.data.at(0) + bufferView.byteOffset,
                GL_STATIC_DRAW
            );
            */
        }

        // Create vertex buffer layout
        for (size_t i = 0; i < gltfMesh.primitives.size(); ++i)
        {
            tinygltf::Primitive primitive = gltfMesh.primitives[i];
            tinygltf::Accessor indexAccessor = gltfModel.accessors[primitive.indices];

            // TODO:
            // go through all attributes and build VertexAttributeLayout
            // from those for the buffer above!
            for (auto &attrib : primitive.attributes)
            {
                tinygltf::Accessor accessor = gltfModel.accessors[attrib.second];
                int byteStride = accessor.ByteStride(gltfModel.bufferViews[accessor.bufferView]);
                glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

                int size = 1;
                if (accessor.type != TINYGLTF_TYPE_SCALAR)
                {
                    size = accessor.type;
                }

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
                    ShaderDataType componentType = gltf_accessor_component_type_to_engine(accessor.componentType);

                    glEnableVertexAttribArray(vaa);
                    glVertexAttribPointer(vaa, size, accessor.componentType,
                            accessor.normalized ? GL_TRUE : GL_FALSE,
                            byteStride, BUFFER_OFFSET(accessor.byteOffset));
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
    }

Model* load_model_glb(const std::string& filepath)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string error;
    std::string warning;

    //bool ret = loader.LoadASCIIFromFile(&model, &error, &warning, filepath);
    bool ret = loader.LoadBinaryFromFile(&model, &error, &warning, filepath); // for binary glTF(.glb)

    // just to return non nullptr if loaded successfully
    Mesh* pMesh = new Mesh(nullptr, nullptr, nullptr);
    Model* pModel = new Model({ pMesh });

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


    return pModel;
}
}
