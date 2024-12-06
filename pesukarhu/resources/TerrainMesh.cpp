#include "TerrainMesh.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
    TerrainMesh::TerrainMesh(
        Buffer* pVertexBuffer,
        Buffer* pIndexBuffer,
        TerrainMaterial* pMaterial
    ) :
        Resource(ResourceType::RESOURCE_TERRAIN_MESH),
        _pVertexBuffer(pVertexBuffer),
        _pIndexBuffer(pIndexBuffer),
        _pMaterial(pMaterial)
    {
    }

    TerrainMesh::~TerrainMesh()
    {
        if (_pVertexBuffer)
            delete _pVertexBuffer;
        if (_pIndexBuffer)
            delete _pIndexBuffer;
    }
}
