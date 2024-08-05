#include "Model.h"
#include "core/Debug.h"

namespace pk
{

    Model::Model(std::vector<Mesh*> pMeshes) :
        Resource(ResourceType::RESOURCE_MODEL),
        _pMeshes(pMeshes)
    {}

    Model::Model(std::vector<Mesh*> pMeshes, Pose bindPose) :
        Resource(ResourceType::RESOURCE_MODEL),
        _pMeshes(pMeshes),
        _bindPose(bindPose)
    {}

    const Mesh * const Model::getMesh(int index) const
    {
        if (index >= _pMeshes.size())
        {
            Debug::log(
                "@Model::getMesh "
                "Index " + std::to_string(index) + " "
                "this mesh has: " + std::to_string(_pMeshes.size()) + " meshes!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        return _pMeshes[index];
    }
}
