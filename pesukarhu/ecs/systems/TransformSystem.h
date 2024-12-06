#pragma once

#include "System.h"
#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/ecs/components/ComponentPool.h"
#include "pesukarhu/ecs/components/Transform.h"

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
