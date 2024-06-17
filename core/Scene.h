#pragma once

#include "../ecs/components/Component.h"
#include "../ecs/components/Camera.h"
#include "../ecs/systems/System.h"
#include "Debug.h"
#include <unordered_map>
#include <vector>

// NOTE: Only temporarely adding all systems here on Scene's constructor!
#include "ecs/systems/ui/ConstraintSystem.h"
#include "ecs/systems/rendering/GUIRenderer.h"


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
        std::unordered_map<ComponentType, std::vector<uint32_t>> typeComponentMapping;

        Camera* activeCamera = nullptr;

        Scene()
        {
            // NOTE: Only temporarely adding all default systems here!
            // TODO: Some better way of handling this!!
            //  Also you would need to create all default systems at start
            //  and never even destroy them..
            systems.push_back(new ui::ConstraintSystem);
            systems.push_back(new GUIRenderer);
        }

        virtual ~Scene()
        {
            Debug::log("Destroying components...");
            for (const std::pair<uint32_t, Component*> c : components)
                delete c.second;

            components.clear();
            entities.clear();

            for (System* system : systems)
                delete system;

            systems.clear();

            _entityChildMapping.clear();
        }

        uint32_t createEntity(std::vector<uint32_t> children = {})
        {
            // *Start entity ids from 1 and NOT from 0
            uint32_t id = entities.size() + 1;
            entities.push_back(id);
            _entityChildMapping[id] = children;

            return id;
        }

        void addChild(uint32_t entity, uint32_t child)
        {
            _entityChildMapping[entity].push_back(child);
        }

        std::vector<uint32_t> getChildren(uint32_t entity)
        {
            return _entityChildMapping[entity];
        }

        void addComponent(uint32_t entity, Component* component)
        {
            component->_entity = entity;
        }

        //void addSystem(System* system)
        //{
        //    systems.push_back(system);
        //}

        // Returns first component of "type" associated with "entity"
        // TODO: Some kind of entity - component mapping to speed this up?
        Component* getComponent(uint32_t entity, ComponentType type, bool nestedSearch = false)
        {
            for (uint32_t componentID : typeComponentMapping[type])
            {
                Component* pComponent = components[componentID];
                if (pComponent->getEntity() == entity)
                    return pComponent;
            }
            if (!nestedSearch)
                Debug::log("Couldn't find component of type: " + std::to_string(type) + " from entity: " + std::to_string(entity), Debug::MessageType::PK_MESSAGE);
            return nullptr;
        }

        // Returns first component of "type" found in "entity"'s child entities
        Component* getComponentInChildren(uint32_t entity, ComponentType type)
        {
            for (const uint32_t& child : _entityChildMapping[entity])
            {
                Component* pComponent = getComponent(child, type, true);
                if (pComponent)
                    return pComponent;
            }
            Debug::log("Couldn't find component of type: " + std::to_string(type) + " from child entities of entity: " + std::to_string(entity), Debug::MessageType::PK_MESSAGE);
            return nullptr;
        }

        // Return all components of entity
        // TODO: Some kind of entity - component mapping to speed this up?
        std::vector<Component*> getComponents(uint32_t entity)
        {
            std::vector<Component*> foundComponents;
            for (const std::pair<uint32_t, Component*> component : components)
            {
                Component* pComponent = component.second;
                if (pComponent->getEntity() == entity)
                    foundComponents.push_back(pComponent);
            }
            return foundComponents;
        }

        // Returns first found component of type "type"
        // TODO: Make this more safe?
        Component* getComponent(ComponentType type)
        {
            auto iter = typeComponentMapping.find(type);
            if (iter != typeComponentMapping.end())
            {
                if (typeComponentMapping[type].size() > 0)
                    return components[typeComponentMapping[type][0]];
            }
            return nullptr;
        }

        std::vector<Component*> getComponentsInChildren(uint32_t entity)
        {
            std::vector<Component*> childComponents;
            for (uint32_t childEntity : getChildren(entity))
            {
                for (Component* c : getComponents(childEntity))
                    childComponents.push_back(c);
            }
            return childComponents;
        }

        // Returns all components of entity and its' children all the way down the hierarchy
        std::vector<Component*> getAllComponents(uint32_t entity)
        {
            std::vector<Component*> ownComponents = getComponents(entity);
            for (uint32_t e : _entityChildMapping[entity])
            {
                std::vector<Component*> childComponents = getAllComponents(e);
                for (Component* c : childComponents)
                    ownComponents.push_back(c);
            }
            return ownComponents;
        }

        // Returns all components in scene of specific type
        std::vector<Component*> getComponentsOfTypeInScene(ComponentType type)
        {
            std::vector<Component*> foundComponents;
            for (uint32_t componentID : typeComponentMapping[type])
                foundComponents.push_back(components[componentID]);
            return foundComponents;
        }

        virtual void init() = 0;
        virtual void update() = 0;
    };
}
