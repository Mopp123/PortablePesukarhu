#include "Scene.h"
#include "Application.h"

// NOTE: Only temporarely adding all systems here on Scene's constructor!
#include "ecs/systems/ui/ConstraintSystem.h"

namespace pk
{
    Scene::Scene()
    {
        // NOTE: Only temporarely adding all default systems here!
        // TODO: Some better way of handling this!!
        //  Also you would need to create all default systems at start
        //  and never even destroy them..
        systems.push_back(new ui::ConstraintSystem);
    }

    Scene::~Scene()
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

    uint32_t Scene::createEntity(std::vector<uint32_t> children)
    {
        // *Start entity ids from 1 and NOT from 0
        uint32_t id = entities.size() + 1;
        entities.push_back(id);
        _entityChildMapping[id] = children;

        return id;
    }

    void Scene::addChild(uint32_t entity, uint32_t child)
    {
        _entityChildMapping[entity].push_back(child);
    }

    std::vector<uint32_t> Scene::getChildren(uint32_t entity)
    {
        return _entityChildMapping[entity];
    }

    void Scene::addComponent(uint32_t entity, Component* component)
    {
        uint32_t componentID = component->getID();
        ComponentType componentType = component->_type;

        component->_id = componentID;
        components[componentID] = component;
        typeComponentMapping[componentType].push_back(componentID);

        component->_entity = entity;

        Renderer* pRenderer = Application::get()->getMasterRenderer()->getRenderer(componentType);
        if (pRenderer)
        {
            pRenderer->createDescriptorSets(component);
        }
    }

    //void addSystem(System* system)
    //{
    //    systems.push_back(system);
    //}

    // Returns first component of "type" associated with "entity"
    // TODO: Some kind of entity - component mapping to speed this up?
    Component* Scene::getComponent(uint32_t entity, ComponentType type, bool nestedSearch)
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
    Component* Scene::getComponentInChildren(uint32_t entity, ComponentType type)
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
    std::vector<Component*> Scene::getComponents(uint32_t entity)
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
    Component* Scene::getComponent(ComponentType type)
    {
        auto iter = typeComponentMapping.find(type);
        if (iter != typeComponentMapping.end())
        {
            if (typeComponentMapping[type].size() > 0)
                return components[typeComponentMapping[type][0]];
        }
        return nullptr;
    }

    std::vector<Component*> Scene::getComponentsInChildren(uint32_t entity)
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
    std::vector<Component*> Scene::getAllComponents(uint32_t entity)
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
    std::vector<Component*> Scene::getComponentsOfTypeInScene(ComponentType type)
    {
        std::vector<Component*> foundComponents;
        for (uint32_t componentID : typeComponentMapping[type])
            foundComponents.push_back(components[componentID]);
        return foundComponents;
    }
}
