#include "TransformSystem.h"
#include "core/Scene.h"
#include <unordered_map>


namespace pk
{
    TransformSystem::TransformSystem()
    {}

    TransformSystem::~TransformSystem()
    {}

    void TransformSystem::update(Scene* pScene)
    {
        _pScene = pScene;
        std::unordered_map<ComponentType, ComponentPool>::iterator poolIt = pScene->componentPools.find(ComponentType::PK_TRANSFORM);
        if (poolIt == pScene->componentPools.end())
        {
            Debug::log(
                "@TransformSystem::update "
                "Scene doesn't have component pool of type: " + std::to_string(ComponentType::PK_TRANSFORM),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        _pTransformPool = &poolIt->second;

        for (Entity e : pScene->entities)
        {
            // Atm disable this system for entities which has ui constraints
            // REASON: If transform is constrained -> u cant fucking touch it!(unless through the constraint)
            if ((e.componentMask & ComponentType::PK_TRANSFORM) && !(e.componentMask & ComponentType::PK_UI_CONSTRAINT ))
            {
                if (e.parentID == NULL_ENTITY_ID)
                {
                    Transform* pTransform = (Transform*)((*_pTransformPool)[e.id]);
                    applyTransforms(e.id, pTransform);
                }
            }
        }
    }

    void TransformSystem::applyTransforms(entityID_t parent, Transform* pParentTransform)
    {
        for (entityID_t child : _pScene->entityChildMapping[parent])
        {
            Transform* pChildTransform = (Transform*)((*_pTransformPool)[child]);
            // NOTE: wasnt used anymore?
            // TODO: delete below..
            pChildTransform->_hasParent = true;

            mat4& childLocalMat = pChildTransform->accessLocalTransformationMatrix();
            mat4& childMat = pChildTransform->accessTransformationMatrix();

            childMat = pParentTransform->getTransformationMatrix() * childLocalMat;

            if (_pScene->entityChildMapping.find(child) != _pScene->entityChildMapping.end())
                applyTransforms(child, pChildTransform);
        }
    }
}
