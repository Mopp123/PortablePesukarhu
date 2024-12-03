#pragma once

#include "System.h"
#include "Pesukarhu/ecs/Entity.h"
#include "Pesukarhu/ecs/components/ComponentPool.h"
#include "Pesukarhu/ecs/components/Transform.h"

namespace pk
{
    class TransformSystem : public System
    {
    private:
        ComponentPool* _pTransformPool = nullptr;
        Scene* _pScene = nullptr;

    public:
        TransformSystem();
        ~TransformSystem();

        virtual void update(Scene* pScene);

    private:
        void applyTransforms(entityID_t parent, Transform* pParentTransform);

    };
}
