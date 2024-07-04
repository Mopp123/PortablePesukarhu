#pragma once

#include "Memory.h"
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
        std::unordered_map<uint32_t, std::vector<uint32_t>> _entityChildMapping;

    public:
        //std::unordered_map<ComponentType, std::vector<Component*>> components;
        std::vector<System*> systems;
        std::vector<uint32_t> entities;
        std::unordered_map<uint32_t, Component*> components;
        std::unordered_map<ComponentType, MemoryPool> componentPools;

        std::unordered_map<ComponentType, std::vector<uint32_t>> typeComponentMapping;

        Camera* activeCamera = nullptr;

        Scene();
        virtual ~Scene();

        uint32_t createEntity(std::vector<uint32_t> children = {});

        void addChild(uint32_t entity, uint32_t child);

        std::vector<uint32_t> getChildren(uint32_t entity);

        void addComponent(uint32_t entity, Component* component);

        // Returns first component of "type" associated with "entity"
        // TODO: Some kind of entity - component mapping to speed this up?
        Component* getComponent(uint32_t entity, ComponentType type, bool nestedSearch = false);

        // Returns first component of "type" found in "entity"'s child entities
        Component* getComponentInChildren(uint32_t entity, ComponentType type);

        // Return all components of entity
        // TODO: Some kind of entity - component mapping to speed this up?
        std::vector<Component*> getComponents(uint32_t entity);

        // Returns first found component of type "type"
        // TODO: Make this more safe?
        Component* getComponent(ComponentType type);

        std::vector<Component*> getComponentsInChildren(uint32_t entity);

        // Returns all components of entity and its' children all the way down the hierarchy
        std::vector<Component*> getAllComponents(uint32_t entity);

        // Returns all components in scene of specific type
        std::vector<Component*> getComponentsOfTypeInScene(ComponentType type);

        virtual void init() = 0;
        virtual void update() = 0;
    };
}
