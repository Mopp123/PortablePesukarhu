#include "ModelLoading.h"
#include "core/Debug.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "tiny_gltf.h"


namespace pk
{

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
