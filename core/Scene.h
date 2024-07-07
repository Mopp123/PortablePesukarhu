#pragma once

#include "ecs/components/ComponentPool.h"
#include "ecs/Entity.h"
#include "../ecs/components/Component.h"
#include "../ecs/components/Camera.h"
#include "../ecs/systems/System.h"
#include "Debug.h"
#include <unordered_map>
#include <vector>


namespace pk
{
    class Scene
    {
    private:
        // key = parent entity : val = all this entity's children
        std::unordered_map<entityID_t, std::vector<entityID_t>> _entityChildMapping;

    public:
        //std::unordered_map<ComponentType, std::vector<Component*>> components;
        std::vector<System*> systems;
        std::vector<Entity> entities;
        std::vector<entityID_t> freeEntityIDs;
        std::unordered_map<ComponentType, ComponentPool> componentPools;

        // TODO: delete below?
        //std::unordered_map<uint32_t, Component*> components;

        // TODO: delete below
        //std::unordered_map<ComponentType, std::vector<uint32_t>> typeComponentMapping;

        Camera* activeCamera = nullptr;

        Scene();
        virtual ~Scene();

        entityID_t createEntity();
        void destroyEntity(entityID_t entityID);
        void addChild(entityID_t entityID, entityID_t childID);
        std::vector<entityID_t> getChildren(entityID_t entityID);
        void addComponent(entityID_t entityID, Component* component);
        inline bool isValidEntity(entityID_t entityID)
        {
            if (entityID < 0 || entityID >= entities.size())
                return false;
            return entities[entityID].id != NULL_ENTITY_ID;
        }

        // TODO: all getComponent things could be optimized?
        Component* getComponent(entityID_t entityID, ComponentType type, bool nestedSearch = false);
        // Returns first component of "type" found in "entity"'s child entities
        Component* getComponentInChildren(entityID_t entityID, ComponentType type);

        // Return all components of entity
        std::vector<Component*> getComponents(entityID_t entityID);

        // Returns first found component of type "type"
        // TODO: Make this more safe?
        //Component* getComponent(ComponentType type);

        //std::vector<Component*> getComponentsInChildren(uint32_t entity);

        // Returns all components of entity and its' children all the way down the hierarchy
        //std::vector<Component*> getAllComponents(uint32_t entity);

        // Returns all components in scene of specific type
        //std::vector<Component*> getComponentsOfTypeInScene(ComponentType type);

        virtual void init() = 0;
        virtual void update() = 0;
    };
}
