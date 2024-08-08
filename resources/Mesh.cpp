#include "Mesh.h"
#include "core/Debug.h"


namespace pk
{
    Mesh::Mesh(
        Buffer* pVertexBuffer,
        Buffer* pIndexBuffer,
        Material* pMaterial,
        VertexBufferLayout vertexBufferLayout
    ) :
        Resource(ResourceType::RESOURCE_MESH),
        _pIndexBuffer(pIndexBuffer),
        _pMaterial(pMaterial),
        _vertexBufferLayout(vertexBufferLayout)
    {
        _vertexBuffers.push_back(pVertexBuffer);
        if (!_vertexBufferLayout.isValid())
        {
            Debug::log(
                "@Mesh::Mesh(1) "
                "Vertex Buffer Layout contained elements sharing same location!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }
    }

    Mesh::Mesh(
        std::vector<Buffer*>& vertexBuffers,
        Buffer* pIndexBuffer,
        Material* pMaterial,
        VertexBufferLayout vertexBufferLayout
    ) :
        Resource(ResourceType::RESOURCE_MESH),
        _pIndexBuffer(pIndexBuffer),
        _pMaterial(pMaterial),
        _vertexBufferLayout(vertexBufferLayout)
    {
        _vertexBuffers.resize(vertexBuffers.size());
        for (size_t i = 0; i < vertexBuffers.size(); ++i)
            _vertexBuffers[i] = vertexBuffers[i];

        if (!_vertexBufferLayout.isValid())
        {
            Debug::log(
                "@Mesh::Mesh(1) "
                "Vertex Buffer Layout contained elements sharing same location!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }
    }

    Mesh::~Mesh()
    {
        for (Buffer* pB : _vertexBuffers)
            delete pB;
        if (_pIndexBuffer)
            delete _pIndexBuffer;
    }
}
