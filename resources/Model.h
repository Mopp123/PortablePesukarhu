#pragma once

#include "Resource.h"
#include "Mesh.h"
#include "Material.h"
#include "graphics/animation/Pose.h"
#include <vector>

namespace pk
{
    class Model : public Resource
    {
    private:
        // TODO: continuous mesh allocation and change this to be just ptr to that data
        std::vector<Mesh*> _pMeshes;
        // TODO: Atm just testing skeleton here. Put somewhere else!
        Pose _bindPose;

    public:
        Model(std::vector<Mesh*> pMeshes);
        Model(std::vector<Mesh*> pMeshes, Pose bindPose);

        // Currently not used until:
        // TODO: "engine internal asset file format"
        virtual void load() {}
        virtual void save() {}

        const Mesh * const getMesh(int index) const;
        Mesh* accessMesh(int index) { return _pMeshes[index]; }
        std::vector<Mesh*>& accessMeshes() { return _pMeshes; }
        Pose& accessBindPose() { return _bindPose; }
    };
}
