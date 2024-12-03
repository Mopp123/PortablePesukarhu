#pragma once

#include "Pesukarhu/ecs/components/ComponentPool.h"
#include "Pesukarhu/ecs/Entity.h"

#include "Pesukarhu/ecs/components/Component.h"
#include "Pesukarhu/ecs/components/Camera.h"
#include "Pesukarhu/ecs/components/ui/ConstraintData.h"
#include "Pesukarhu/ecs/components/ui/UIElemState.h"
#include "Pesukarhu/ecs/components/renderable/GUIRenderable.h"
#include "Pesukarhu/ecs/components/renderable/TextRenderable.h"
#include "Pesukarhu/ecs/components/renderable/Static3DRenderable.h"
#include "Pesukarhu/ecs/components/renderable/SkinnedRenderable.h"
#include "Pesukarhu/ecs/components/renderable/TerrainRenderable.h"
#include "Pesukarhu/ecs/components/lighting/Lights.h"
#include "Pesukarhu/ecs/components/AnimationData.h"
#include "Pesukarhu/ecs/components/ui/Blinker.h"
#include "Pesukarhu/graphics/Environment.h"
#include "Pesukarhu/graphics/animation/Pose.h"

#include "Pesukarhu/ecs/systems/System.h"
#include "Debug.h"
#include <unordered_map>
#include <vector>


namespace pk
{
    class Scene
    {
    private:

    public:
        //std::unordered_map<ComponentType, std::vector<Component*>> components;
        std::vector<System*> systems;
        std::vector<Entity> entities;
        std::unordered_map<entityID_t, std::vector<entityID_t>> entityChildMapping;
        std::vector<entityID_t> freeEntityIDs;
        std::unordered_map<ComponentType, ComponentPool> componentPools;

        entityID_t activeCamera = NULL_ENTITY_ID;
        entityID_t directionalLight = NULL_ENTITY_ID;
        struct EnvironmentProperties environmentProperties;

        Scene();
        virtual ~Scene();

        entityID_t createEntity();
        // TODO: Some better way of dealing with this
        entityID_t createSkeletonEntity(entityID_t target, const Pose& bindPose);
        Entity getEntity(entityID_t entity) const;
        void destroyEntity(entityID_t entityID);
        void addChild(entityID_t entityID, entityID_t childID);
        // NOTE: Could be optimized to return just ptr to first child and child count
        std::vector<entityID_t> getChildren(entityID_t entityID) const;

        void addComponent(entityID_t entityID, Component* component);
        bool isValidEntity(entityID_t entityID) const;

        // TODO: all getComponent things could be optimized?
        Component* getComponent(
            entityID_t entityID,
            ComponentType type,
            bool nestedSearch = false,
            bool enableWarning = true
        );
        const Component * const getComponent(entityID_t entityID, ComponentType type, bool nestedSearch = false) const;
        // Returns first component of "type" found in "entity"'s child entities
        Component* getComponentInChildren(entityID_t entityID, ComponentType type);

        // Return all components of entity
        std::vector<Component*> getComponents(entityID_t entityID);

        // Returns first found component of type "type"
        //Component* getComponent(ComponentType type);

        //std::vector<Component*> getComponentsInChildren(uint32_t entity);

        // Returns all components of entity and its' children all the way down the hierarchy
        std::vector<Component*> getAllComponents(entityID_t entity);

        // Returns all components in scene of specific type
        //std::vector<Component*> getComponentsOfTypeInScene(ComponentType type);

        entityID_t getActiveCamera() const { return activeCamera; }

        virtual void init() = 0;
        virtual void update() = 0;
        virtual void lateUpdate() {};
    };
}
