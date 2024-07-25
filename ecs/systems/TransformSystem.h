#pragma once

#include "System.h"
#include "ecs/Entity.h"
#include "ecs/components/ComponentPool.h"
#include "ecs/components/Transform.h"

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
