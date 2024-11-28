#include "Mesh.h"
#include "core/Debug.h"


namespace pk
{
    Mesh::Mesh(
        const std::vector<Buffer*>& vertexBuffers,
        Buffer* pIndexBuffer,
        Material* pMaterial
    ) :
        Resource(ResourceType::RESOURCE_MESH),
        _pIndexBuffer(pIndexBuffer),
        _pMaterial(pMaterial)
    {
        _vertexBuffers.resize(vertexBuffers.size());
        for (size_t i = 0; i < vertexBuffers.size(); ++i)
            _vertexBuffers[i] = vertexBuffers[i];
    }

    Mesh::Mesh(
        const std::vector<Buffer*>& vertexBuffers,
        Buffer* pIndexBuffer,
        Material* pMaterial,
        const Pose& bindPose,
        const std::vector<Pose>& animPoses
    ) :
        Resource(ResourceType::RESOURCE_MESH),
        _pIndexBuffer(pIndexBuffer),
        _pMaterial(pMaterial),
        _bindPose(bindPose),
        _animPoses(animPoses)
    {
        _vertexBuffers.resize(vertexBuffers.size());
        for (size_t i = 0; i < vertexBuffers.size(); ++i)
            _vertexBuffers[i] = vertexBuffers[i];
    }

    Mesh::~Mesh()
    {
        for (Buffer* pB : _vertexBuffers)
            delete pB;
        if (_pIndexBuffer)
            delete _pIndexBuffer;
    }
}
