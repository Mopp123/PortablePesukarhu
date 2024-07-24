#include "Mesh.h"

namespace pk
{
    Mesh::Mesh(
        Buffer* pVertexBuffer,
        Buffer* pIndexBuffer,
        Material* pMaterial,
        VertexBufferLayout vertexBufferLayout
    ) :
        Resource(ResourceType::RESOURCE_MESH),
        _pVertexBuffer(pVertexBuffer),
        _pIndexBuffer(pIndexBuffer),
        _pMaterial(pMaterial),
        _vertexBufferLayout(vertexBufferLayout)
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
