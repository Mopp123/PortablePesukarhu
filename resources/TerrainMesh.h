#pragma once

#include "Resource.h"
#include "graphics/Buffers.h"
#include "graphics/animation/Pose.h"
#include "TerrainMaterial.h"
#include <vector>


namespace pk
{
    class TerrainMesh : public Resource
    {
    private:
        Buffer* _pVertexBuffer = nullptr;
        Buffer* _pIndexBuffer = nullptr;
        TerrainMaterial* _pMaterial = nullptr;

    public:
        // NOTE: buffers' ownerships gets transferred to here!
        TerrainMesh(
            Buffer* pVertexBuffer,
            Buffer* pIndexBuffer,
            TerrainMaterial* pMaterial
        );
        TerrainMesh(const TerrainMesh&) = delete;
        ~TerrainMesh();

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        inline const Buffer* getVertexBuffer() const { return _pVertexBuffer; }
        inline Buffer* accessVertexBuffer() { return _pVertexBuffer; }

        inline const Buffer * const getIndexBuffer() const { return _pIndexBuffer; }
        inline const TerrainMaterial * const getTerrainMaterial() const { return _pMaterial; }
        // TODO: Get rid of most of this access/get shit and update all possible places
        // to take in as const!!!
        inline TerrainMaterial* accessTerrainMaterial() { return _pMaterial; }
    };
}
