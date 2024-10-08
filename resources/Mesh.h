#pragma once

#include "Resource.h"
#include "graphics/Buffers.h"
#include "graphics/animation/Pose.h"
#include "Material.h"
#include <vector>


namespace pk
{
    class Mesh : public Resource
    {
    private:
        std::vector<Buffer*> _vertexBuffers;
        Buffer* _pIndexBuffer = nullptr;
        Material* _pMaterial = nullptr;
        VertexBufferLayout _vertexBufferLayout; // Not sure should this actually even be part of the mesh...

        // These aren't required. Mesh may be skinned or not..
        Pose _bindPose;
        std::vector<Pose> _animPoses;

    public:
        // NOTE: buffers' ownerships gets transferred to here!
        Mesh(
            const std::vector<Buffer*>& vertexBuffers,
            Buffer* pIndexBuffer,
            Material* pMaterial
        );
        Mesh(
            const std::vector<Buffer*>& vertexBuffers,
            Buffer* pIndexBuffer,
            Material* pMaterial,
            VertexBufferLayout vertexBufferLayout // TODO: remove layout from mesh
        );
        Mesh(
            const std::vector<Buffer*>& vertexBuffers,
            Buffer* pIndexBuffer,
            Material* pMaterial,
            VertexBufferLayout vertexBufferLayout,
            const Pose& bindPose,
            const std::vector<Pose>& animPoses
        );
        Mesh(const Mesh&) = delete;
        ~Mesh();

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        // TODO: Remove below (get buffers only by index)
        inline const Buffer* getVertexBuffer() const { return _vertexBuffers[0]; }
        inline Buffer* accessVertexBuffer() { return _vertexBuffers[0]; }
        // NOTE: Start using below instead of above
        inline const Buffer* getVertexBuffer_DANGER(int index) const { return _vertexBuffers[index]; }
        inline Buffer* accessVertexBuffer_DANGER(int index) { return _vertexBuffers[index]; }

        inline const Buffer * const getIndexBuffer() const { return _pIndexBuffer; }
        inline const Material * const getMaterial() const { return _pMaterial; }
        // TODO: Get rid of most of this access/get shit and update all possible places
        // to take in as const!!!
        inline  Material* accessMaterial() { return _pMaterial; }
        inline  void setMaterial(Material* pMaterial) { _pMaterial = pMaterial; }

        inline Pose& accessBindPose() { return _bindPose; }
        inline const Pose& getBindPose() const { return _bindPose; }
        inline std::vector<Pose>& accessAnimPoses() { return _animPoses; }
        inline const std::vector<Pose>& getAnimPoses() const { return _animPoses; }
        // TODO: get rid of below after cleaning model loading..
        inline void setBindPose(const Pose& pose) { _bindPose = pose; }
        inline void setAnimationPoses(const std::vector<Pose>& animPoses) { _animPoses = animPoses; }
    };
}
