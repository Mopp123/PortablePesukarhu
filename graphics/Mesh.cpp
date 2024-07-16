#include "Mesh.h"

namespace pk
{
    Mesh::Mesh(
        Buffer* pVertexBuffer,
        Buffer* pIndexBuffer,
        Material* pMaterial
    ) :
        Resource(ResourceType::RESOURCE_MESH),
        _pVertexBuffer(pVertexBuffer),
        _pIndexBuffer(pIndexBuffer),
        _pMaterial(pMaterial)
    {
    }

    Mesh::~Mesh()
    {
        if (_pVertexBuffer)
            delete _pVertexBuffer;
        if (_pIndexBuffer)
            delete _pIndexBuffer;
    }
}
