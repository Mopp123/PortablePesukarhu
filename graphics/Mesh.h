#pragma once

#include "core/Resource.h"
#include "Buffers.h"
#include "Material.h"

namespace pk
{
    class Mesh : public Resource
    {
    private:
        Buffer* _pVertexBuffer = nullptr;
        Buffer* _pIndexBuffer = nullptr;
        Material* _pMaterial = nullptr;
        VertexBufferLayout _vertexBufferLayout;

    public:
        // NOTE: buffers' ownership gets transferred to here!
        Mesh(
            Buffer* pVertexBuffer,
            Buffer* pIndexBuffer,
            Material* pMaterial,
            VertexBufferLayout vertexBufferLayout
        );
        Mesh(const Mesh&) = delete;
        ~Mesh();

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        inline const Buffer* getVertexBuffer() const { return _pVertexBuffer; }
        inline Buffer* accessVertexBuffer() { return _pVertexBuffer; }
        inline const Buffer * const getIndexBuffer() const { return _pIndexBuffer; }
        inline const Material * const getMaterial() const { return _pMaterial; }
        // TODO: Get rid of most of this access/get shit and update all possible places
        // to take in as const!!!
        inline  Material* accessMaterial() { return _pMaterial; }
        inline  void setMaterial(Material* pMaterial) { _pMaterial = pMaterial; }
    };
}
