#include "Scene.h"
#include "Application.h"

#include "ecs/components/renderable/Sprite3DRenderable.h"
#include "ecs/components/renderable/TerrainTileRenderable.h"
#include "ecs/components/lighting/Lights.h"

// NOTE: Only temporarely adding all systems here on Scene's constructor!
#include "ecs/systems/ui/ConstraintSystem.h"
#include "ecs/systems/TransformSystem.h"
#include <unordered_map>

namespace pk
{
    Scene::Scene()
    {
        size_t maxEntityCount = 1000;
        size_t maxDirectionalLights = 1;

        componentPools[ComponentType::PK_TRANSFORM] = ComponentPool(
            sizeof(Transform), maxEntityCount, true
        );
        componentPools[ComponentType::PK_CAMERA] = ComponentPool(
            sizeof(Camera), 1, true
        );
        componentPools[ComponentType::PK_UI_CONSTRAINT] = ComponentPool(
            sizeof(ConstraintData), 10, true
        );
        componentPools[ComponentType::PK_UIELEM_STATE] = ComponentPool(
            sizeof(UIElemState), 10, true
        );
        componentPools[ComponentType::PK_LIGHT_DIRECTIONAL] = ComponentPool(
            sizeof(DirectionalLight), 4, true
        );
        // Renderable component pools
        componentPools[ComponentType::PK_RENDERABLE_GUI] = ComponentPool(
            sizeof(GUIRenderable), 100, true
        );
        componentPools[ComponentType::PK_RENDERABLE_TEXT] = ComponentPool(
            sizeof(TextRenderable), 100, true
        );
        componentPools[ComponentType::PK_RENDERABLE_SPRITE3D] = ComponentPool(
            sizeof(Sprite3DRenderable), 100, true
        );
        componentPools[ComponentType::PK_RENDERABLE_STATIC3D] = ComponentPool(
            sizeof(Static3DRenderable), 100, true
        );
        componentPools[ComponentType::PK_RENDERABLE_TERRAINTILE] = ComponentPool(
            sizeof(TerrainTileRenderable), 100, true
        );
        componentPools[ComponentType::PK_LIGHT_DIRECTIONAL] = ComponentPool(
            sizeof(DirectionalLight), maxDirectionalLights, true
        );

        // NOTE: Only temporarely adding all default systems here!
        // TODO: Some better way of handling this!!
        //  Also you would need to create all default systems at start
        //  and never even destroy them..
        systems.push_back(new ui::ConstraintSystem);
        systems.push_back(new TransformSystem);
    }

    Scene::~Scene()
    {
        std::unordered_map<ComponentType, ComponentPool>::iterator poolIterator;
        for (poolIterator = componentPools.begin(); poolIterator != componentPools.end(); ++poolIterator)
            poolIterator->second.freeStorage();

        entities.clear();

        for (System* system : systems)
            delete system;

        systems.clear();

        entityChildMapping.clear();
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
        // Destroy/free all this entity's components
        uint64_t componentMask = entities[entityID].componentMask;
        std::unordered_map<ComponentType, ComponentPool>::iterator poolsIt;
        for (poolsIt = componentPools.begin(); poolsIt != componentPools.end(); ++poolsIt)
        {
            if (componentMask & poolsIt->first)
                poolsIt->second.destroyComponent(entityID);
        }
        // Destroy/free entity itself
        freeEntityIDs.push_back(entityID);
        entities[entityID].clear();

        if (entityChildMapping.find(entityID) != entityChildMapping.end())
        {
            for (entityID_t childID : entityChildMapping[entityID])
            {
                destroyEntity(childID);
            }
            entityChildMapping.erase(entityID);
        }
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
        entities[childID].parentID = entityID;
        entityChildMapping[entityID].push_back(childID);
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
        return entityChildMapping[entityID];
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
                "Attempted to add component to entity: " + std::to_string(entityID) + " but "
                "it already has a component of this type: " + std::to_string(componentTypeID),
                Debug::PK_FATAL_ERROR
            );
            return;
        }
        entity.componentMask |= componentTypeID;
    }

    Transform* Scene::createTransform(entityID_t target, vec2 pos, vec2 scale)
    {
        Transform* pTransform = (Transform*)componentPools[ComponentType::PK_TRANSFORM].allocComponent(target);
        *pTransform = Transform(pos, scale);
        addComponent(target, pTransform);
        return pTransform;
    }

    Transform* Scene::createTransform(
        entityID_t target,
        vec3 pos,
        vec3 scale,
        float pitch,
        float yaw
    )
    {
        Transform* pTransform = (Transform*)componentPools[ComponentType::PK_TRANSFORM].allocComponent(target);
        *pTransform = Transform(pos, scale, pitch, yaw);
        addComponent(target, pTransform);
        return pTransform;
    }

    ConstraintData* Scene::createUIConstraint(
        entityID_t target,
        HorizontalConstraintType horizontalType,
        float horizontalValue,
        VerticalConstraintType verticalType,
        float verticalValue
    )
    {
        ConstraintData* pConstraint = (ConstraintData*)componentPools[ComponentType::PK_UI_CONSTRAINT].allocComponent(target);
        *pConstraint = ConstraintData(horizontalType, horizontalValue, verticalType, verticalValue);
        addComponent(target, pConstraint);
        return pConstraint;
    }

    UIElemState* Scene::createUIElemState(entityID_t target)
    {
        UIElemState* pElemState = (UIElemState*)componentPools[ComponentType::PK_UIELEM_STATE].allocComponent(target);
        *pElemState = UIElemState(); // not sure if fukup here?
        addComponent(target, pElemState);
        return pElemState;
    }

    GUIRenderable* Scene::createGUIRenderable(
        entityID_t target,
        Texture_new* pTexture,
        vec3 color,
        vec4 borderColor,
        float borderThickness,
        vec4 textureCropping
    )
    {
        GUIRenderable* pRenderable = (GUIRenderable*)componentPools[ComponentType::PK_RENDERABLE_GUI].allocComponent(target);
        *pRenderable = GUIRenderable(nullptr, textureCropping, pTexture);
        pRenderable->color = color;
        pRenderable->borderColor = borderColor;
        pRenderable->borderThickness = borderThickness;
        addComponent(target, pRenderable);
        return pRenderable;
    }

    TextRenderable* Scene::createTextRenderable(
        entityID_t target,
        const std::string& txt,
        PK_id fontID,
        vec3 color,
        bool bold
    )
    {
        TextRenderable* pRenderable = (TextRenderable*)componentPools[ComponentType::PK_RENDERABLE_TEXT].allocComponent(target);
        *pRenderable = TextRenderable(txt, fontID, color, bold);
        addComponent(target, pRenderable);
        return pRenderable;
    }

    Static3DRenderable* Scene::createStatic3DRenderable(
        entityID_t target,
        PK_id meshID
    )
    {
        Static3DRenderable* pRenderable = (Static3DRenderable*)componentPools[ComponentType::PK_RENDERABLE_STATIC3D].allocComponent(target);
        *pRenderable = Static3DRenderable(meshID);
        addComponent(target, pRenderable);
        return pRenderable;
    }

    Camera* Scene::createCamera(
        entityID_t target,
        const vec3& position,
        float pitch,
        float yaw
    )
    {
        Application* pApp = Application::get();
        const Window* window = pApp->getWindow();

        const float windowWidth = (float)window->getWidth();
        const float windowHeight = (float)window->getHeight();

        const float aspectRatio = windowWidth / windowHeight;
        mat4 orthographicProjMat = create_proj_mat_ortho(0, windowWidth, windowHeight, 0, 0.0f, 100.0f);
        mat4 perspectivaProjMat = create_perspective_projection_matrix(aspectRatio, 1.3f, 0.1f, 1000.0f);

        Camera* pCamera = (Camera*)componentPools[ComponentType::PK_CAMERA].allocComponent(target);
        *pCamera = Camera(orthographicProjMat, perspectivaProjMat);
        addComponent(target, pCamera);
        createTransform(target, position, { 1, 1, 1 }, pitch, yaw);

        pApp->accessInputManager()->addWindowResizeEvent(new CameraWindowResizeEvent(*pCamera));
        return pCamera;
    }

    DirectionalLight* Scene::createDirectionalLight(
        entityID_t target,
        const vec3& color,
        const vec3& direction
    )
    {
        DirectionalLight* pDirectionalLight = (DirectionalLight*)componentPools[ComponentType::PK_LIGHT_DIRECTIONAL].allocComponent(target);
        *pDirectionalLight = DirectionalLight(color, direction);
        addComponent(target, pDirectionalLight);
        return pDirectionalLight;
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
        if (entities[entityID].componentMask & (uint64_t)type)
            return (Component*)componentPools[type].getComponent_DANGER(entityID);
        if (!nestedSearch)
            Debug::log(
                "Couldn't find component of type: " + std::to_string(type) + " from entity: " + std::to_string(entityID)
            );
        return nullptr;
    }

    const Component * const Scene::getComponent(entityID_t entityID, ComponentType type, bool nestedSearch) const
    {
        if (!isValidEntity(entityID))
        {
            Debug::log(
                "@Scene::getComponent(2) "
                "Invalid entityID!",
                Debug::PK_FATAL_ERROR
            );
            return nullptr;
        }
        std::unordered_map<ComponentType, ComponentPool>::const_iterator itPool = componentPools.find(type);
        if (itPool == componentPools.end())
        {
            Debug::log(
                "@Scene::getComponent(2) "
                "No component pool exists for component type: " + std::to_string(type),
                Debug::PK_FATAL_ERROR
            );
            return nullptr;
        }
        if (entities[entityID].componentMask & (uint64_t)type)
            return (const Component * const)itPool->second.getComponent_DANGER(entityID);
        if (!nestedSearch)
            Debug::log(
                "@Scene::getComponent(2) "
                "Couldn't find component of type: " + std::to_string(type) + " from entity: " + std::to_string(entityID)
            );
        return nullptr;
    }

    // Returns first component of "type" found in "entity"'s child entities
    Component* Scene::getComponentInChildren(entityID_t entityID, ComponentType type)
    {
        for (const entityID_t& child : entityChildMapping[entityID])
        {
            Component* pComponent = getComponent(child, type, true);
            if (pComponent)
                return pComponent;
        }
        Debug::log("Couldn't find component of type: " + std::to_string(type) + " from child entities of entity: " + std::to_string(entityID), Debug::MessageType::PK_MESSAGE);
        return nullptr;
    }

    // Return all components of entity
    std::vector<Component*> Scene::getComponents(entityID_t entityID)
    {
        std::vector<Component*> foundComponents;
        if (!isValidEntity(entityID))
        {
            Debug::log(
                "@Scene::getComponents "
                "Invalid entity: " + std::to_string(entityID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return foundComponents;
        }
        Entity& entity = entities[entityID];
        std::unordered_map<ComponentType, ComponentPool>::iterator it;;
        for (it = componentPools.begin(); it != componentPools.end(); ++it)
        {
            if (entity.componentMask & it->first)
                foundComponents.push_back((Component*)it->second.getComponent_DANGER(entityID));
        }
        return foundComponents;
    }

    // Returns first found component of type "type"
    //Component* Scene::getComponent(ComponentType type)
    //{
    //    auto iter = typeComponentMapping.find(type);
    //    if (iter != typeComponentMapping.end())
    //    {
    //        if (typeComponentMapping[type].size() > 0)
    //            return components[typeComponentMapping[type][0]];
    //    }
    //    return nullptr;
    //}

    //std::vector<Component*> Scene::getComponentsInChildren(uint32_t entity)
    //{
    //    std::vector<Component*> childComponents;
    //    for (uint32_t childEntity : getChildren(entity))
    //    {
    //        for (Component* c : getComponents(childEntity))
    //            childComponents.push_back(c);
    //    }
    //    return childComponents;
    //}

    // Returns all components of entity and its' children all the way down the hierarchy
    //std::vector<Component*> Scene::getAllComponents(uint32_t entity)
    //{
    //    std::vector<Component*> ownComponents = getComponents(entity);
    //    for (uint32_t e : _entityChildMapping[entity])
    //    {
    //        std::vector<Component*> childComponents = getAllComponents(e);
    //        for (Component* c : childComponents)
    //            ownComponents.push_back(c);
    //    }
    //    return ownComponents;
    //}

    // Returns all components in scene of specific type
    //std::vector<Component*> Scene::getComponentsOfTypeInScene(ComponentType type)
    //{
    //    std::vector<Component*> foundComponents;
    //    for (uint32_t componentID : typeComponentMapping[type])
    //        foundComponents.push_back(components[componentID]);
    //    return foundComponents;
    //}
}
