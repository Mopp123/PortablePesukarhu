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

    public:
        Mesh(
            Buffer* pVertexBuffer,
            Buffer* pIndexBuffer,
            Material* pMaterial
        );
        Mesh(const Mesh&) = delete;

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        inline const Buffer * const getVertexBuffer() const { return _pVertexBuffer; }
        inline const Buffer * const getIndexBuffer() const { return _pIndexBuffer; }
        inline const Material * const getMaterial() const { return _pMaterial; }
    };
}
