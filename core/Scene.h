#pragma once

#include "../ecs/components/Component.h"
#include "../ecs/components/Camera.h"
#include "../ecs/systems/System.h"
#include "../ecs/systems/CameraUtils.h"
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
        std::unordered_map<ComponentType, std::vector<Component*>> components;
        std::unordered_map<SystemType, std::vector<System*>> systems;
        std::vector<uint32_t> entities;

        Camera* activeCamera = nullptr;

        Scene() 
        {
        }

        virtual ~Scene() 
        {
            for (const std::pair<ComponentType, std::vector<Component*>>& cContainer : components)
            {
                Debug::log("Destroying components...");
                for (Component* c : cContainer.second)
                    delete c;
            }
            components.clear();
            entities.clear();
            
            for (const std::pair<SystemType, std::vector<System*>>& sysContainer : systems)
            {
                for (System* system : sysContainer.second)
                    delete system;
            }
            
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
            components[component->getType()].push_back(component);
        }

        void addSystem(System* system)
        {
            systems[system->getType()].push_back(system);
        }

        // Returns first component of "type" associated with "entity"
        Component* getComponent(uint32_t entity, ComponentType type, bool nestedSearch = false)
        {
            for (Component* c : components[type])
            {
                if (c->getEntity() == entity)
                    return c;
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
                Component* component = getComponent(child, type, true);
                if (component)
                    return component;
            } 
            Debug::log("Couldn't find component of type: " + std::to_string(type) + " from child entities of entity: " + std::to_string(entity), Debug::MessageType::PK_MESSAGE);
            return nullptr;
        }

        // Return all components of entity
        std::vector<Component*> getComponents(uint32_t entity)
        {
            std::vector<Component*> foundComponents;
            for (const std::pair<ComponentType, std::vector<Component*>>& componentTypes : components)
            {
               const  std::vector<Component*>& componentList = componentTypes.second;
               for (Component* c : componentList)
               {
                   if (c->getEntity() == entity)
                       foundComponents.push_back(c);
               }
            }
            return foundComponents;
        }

        // Returns first found component found of type "type"
        Component* getComponent(ComponentType type)
        {
            auto iter = components.find(type);
            if (iter != components.end())
            {
                if (!iter->second.empty())
                    return iter->second[0];
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

        virtual void init() = 0;
        virtual void update() = 0;
    };	
}
