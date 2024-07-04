#include "Scene.h"
#include "Application.h"

#include "ecs/components/renderable/GUIRenderable.h"

// NOTE: Only temporarely adding all systems here on Scene's constructor!
#include "ecs/systems/ui/ConstraintSystem.h"
#include <unordered_map>

namespace pk
{
    Scene::Scene()
    {
        componentPools[ComponentType::PK_RENDERABLE_GUI] = ComponentPool(
            sizeof(GUIRenderable), 1000
        );

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
        {
            // ATM JUST TESTING HERE:
            // GUIRenderables currently allocated from mem pools
            if (c.second->getType() != ComponentType::PK_RENDERABLE_GUI)
                delete c.second;
        }

        std::unordered_map<ComponentType, ComponentPool>::iterator poolIterator;
        for (poolIterator = componentPools.begin(); poolIterator != componentPools.end(); ++poolIterator)
            poolIterator->second.freeStorage();

        components.clear();
        entities.clear();

        for (System* system : systems)
            delete system;

        systems.clear();

        _entityChildMapping.clear();
    }

    entityID_t Scene::createEntity()
    {
        Entity entity;
        if (!freeEntityIDs.empty())
        {
            entityID_t freeID = freeEntityIDs.back();
            freeEntityIDs.pop_back();

            entity.id = freeID;
            entities[freeID] = entity;
        }
        else
        {
            entity.id = entities.size();
            entities.push_back(entity);
        }
        return entity.id;
    }

    // NOTE: Incomplete and not tested! Propably doesnt work!!
    // TODO: Finish and test this
    void Scene::destroyEntity(entityID_t entityID)
    {
        if (!isValidEntity(entityID))
        {
            Debug::log(
                "@Scene::destroyEntity "
                "Invalid entityID",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        freeEntityIDs.push_back(entityID);
        entities[entityID].id = NULL_ENTITY_ID;
        entities[entityID].componentMask = 0;

        if (_entityChildMapping.find(entityID) != _entityChildMapping.end())
        {
            for (entityID_t childID : _entityChildMapping[entityID])
            {
                destroyEntity(childID);
            }
            _entityChildMapping.erase(entityID);
        }
        // TODO: Make components of this entity "null components"
        Debug::notify_unimplemented(
            "Scene::destroyEntity "
            "Component deletion not implemented!!!"
        );
    }

    void Scene::addChild(entityID_t entityID, entityID_t childID)
    {
        if (!isValidEntity(entityID))
        {
            Debug::log(
                "@Scene::addChild "
                "Invalid entityID: " + std::to_string(entityID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        if (!isValidEntity(childID))
        {
            Debug::log(
                "@Scene::addChild "
                "Invalid childID: " + std::to_string(childID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        _entityChildMapping[entityID].push_back(childID);
    }

    std::vector<entityID_t> Scene::getChildren(entityID_t entityID)
    {
        if (!isValidEntity(entityID))
        {
            Debug::log(
                "@Scene::addChild "
                "Invalid entityID: " + std::to_string(entityID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return {};
        }
        return _entityChildMapping[entityID];
    }

    void Scene::addComponent(entityID_t entityID, Component* component)
    {
        if (!isValidEntity(entityID))
        {
            Debug::log(
                "@Scene::addComponent "
                "Attempted to add component to invalid entity. "
                "Make sure entity hasn't been deleted when calling this.",
                Debug::PK_FATAL_ERROR
            );
            return;
        }
        uint64_t componentTypeID = component->getType();
        Entity& entity = entities[entityID];
        // For now dont allow same component type multiple times in entity
        if (entity.componentMask & componentTypeID)
        {
            Debug::log(
                "@Scene::addComponent "
                "Attempted to add component to but entity already "
                "has a component of this type. Currently entity can have only a "
                "single component for each type",
                Debug::PK_FATAL_ERROR
            );
            return;
        }
        entity.componentMask |= componentTypeID;

        Renderer* pRenderer = Application::get()->getMasterRenderer()->getRenderer(component->getType());
        if (pRenderer)
            pRenderer->createDescriptorSets(component);
    }

    Component* Scene::getComponent(entityID_t entityID, ComponentType type, bool nestedSearch)
    {
        if (!isValidEntity(entityID))
        {
            Debug::log(
                "@Scene::getComponent "
                "Invalid entityID!",
                Debug::PK_FATAL_ERROR
            );
            return nullptr;
        }
        if (componentPools.find(type) == componentPools.end())
        {
            Debug::log(
                "@Scene::getComponent "
                "No component pool exists for component type: " + std::to_string(type),
                Debug::PK_FATAL_ERROR
            );
            return nullptr;
        }
        return (Component*)componentPools[type].getComponent_DANGER(entityID);
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
