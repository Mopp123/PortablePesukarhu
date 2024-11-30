#include "Scene.h"
#include "Application.h"

#include "ecs/components/renderable/Sprite3DRenderable.h"
#include "ecs/components/lighting/Lights.h"

// NOTE: Only temporarely adding all systems here on Scene's constructor!
#include "ecs/systems/ui/ConstraintSystem.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/Animator.h"
#include "ecs/systems/ui/BlinkerSystem.h"
#include <unordered_map>

namespace pk
{
    // result "bones" vector's first is the root entity
    static void create_bone_entity(
        Scene& scene,
        const Pose& pose,
        int currentJointIndex,
        entityID_t parent,
        std::vector<entityID_t>& resultBones
    )
    {
        entityID_t entity = scene.createEntity();
        //if (currentJointIndex == 0)
        //    Debug::log("___TEST___created root joint entity: " + std::to_string(entity));
        resultBones.push_back(entity);
        if (pose.joints.size() <= currentJointIndex)
        {
            Debug::log(
                "@create_bone_entity "
                "Joint index: " + std::to_string(currentJointIndex) + " out of bounds! Joint count: " + std::to_string(pose.joints.size()),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        Joint currentJoint = pose.joints[currentJointIndex];
        vec3 jointTranslation = currentJoint.translation;
        quat jointRotation = currentJoint.rotation;
        // NOTE: Not sure is scale correct here..
        mat4 jointMat = currentJoint.matrix;
        Transform::create(
            entity,
            jointMat
        );

        if (parent != NULL_ENTITY_ID)
            scene.addChild(parent, entity);

        if (pose.jointChildMapping.size() > currentJointIndex)
        {
            for (int childIndex : pose.jointChildMapping[currentJointIndex])
                create_bone_entity(scene, pose, childIndex, entity, resultBones);
        }
    }


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
        componentPools[ComponentType::PK_RENDERABLE_SKINNED] = ComponentPool(
            sizeof(SkinnedRenderable), 100, true
        );
        componentPools[ComponentType::PK_RENDERABLE_TERRAIN] = ComponentPool(
            sizeof(TerrainRenderable), 1, true
        );
        componentPools[ComponentType::PK_LIGHT_DIRECTIONAL] = ComponentPool(
            sizeof(DirectionalLight), maxDirectionalLights, true
        );
        componentPools[ComponentType::PK_ANIMATION_DATA] = ComponentPool(
            sizeof(AnimationData), maxEntityCount, true
        );
        componentPools[ComponentType::PK_BLINKER] = ComponentPool(
            sizeof(Blinker), maxEntityCount, true
        );

        // NOTE: Only temporarely adding all default systems here!
        // TODO: Some better way of handling this!!
        //  Also you would need to create all default systems at start
        //  and never even destroy them..
        systems.push_back(new TransformSystem);
        systems.push_back(new ui::ConstraintSystem);
        systems.push_back(new Animator);
        systems.push_back(new BlinkerSystem);
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

    entityID_t Scene::createSkeletonEntity(entityID_t target, const Pose& bindPose)
    {
        std::vector<entityID_t> result;
        create_bone_entity(
            *this,
            bindPose,
            0,
            target,
            result
        );
        if (result.empty())
        {
            Debug::log(
                "@Scene::createSkeletonEntity "
                "No bones were able to be built! "
                "Input bindPose joint count: " + std::to_string(bindPose.joints.size()) + " "
                "Input bindPose childMapping length: " + std::to_string(bindPose.jointChildMapping.size()),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return NULL_ENTITY_ID;
        }
        return result[0];
    }

    Entity Scene::getEntity(entityID_t entity) const
    {
        Entity outEntity;
        for (const Entity& e : entities)
        {
            if (e.id == entity)
            {
                if (isValidEntity(entity))
                {
                    outEntity = e;
                }
                else
                {
                    Debug::log(
                        "@Scene::getEntity "
                        "Found entity: " + std::to_string(entity) + " "
                        "but the entity was invalid!",
                        Debug::MessageType::PK_ERROR
                    );
                }
                break;
            }
        }
        return outEntity;
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

    // NOTE: Could be optimized to return just ptr to first child and child count
    std::vector<entityID_t> Scene::getChildren(entityID_t entityID) const
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
        if (entityChildMapping.find(entityID) == entityChildMapping.end())
            return {};
        return entityChildMapping.at(entityID);
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
                "it already has a component of this type: " + component_type_to_string(component->getType()),
                Debug::PK_FATAL_ERROR
            );
            return;
        }
        entity.componentMask |= componentTypeID;
    }

    bool Scene::isValidEntity(entityID_t entityID) const
    {
        if (entityID < 0 || entityID >= entities.size())
            return false;
        return entities[entityID].id != NULL_ENTITY_ID;
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
        if ((getEntity(target).componentMask & ComponentType::PK_TRANSFORM) != ComponentType::PK_TRANSFORM)
        {
            Debug::log(
                "@Scene::createStatic3DRenderable "
                "Created renderable component for entity: " + std::to_string(target) + " "
                "with no Transform component! This prevents rendering if transform not specified!",
                Debug::MessageType::PK_WARNING
            );
        }

        Static3DRenderable* pRenderable = (Static3DRenderable*)componentPools[ComponentType::PK_RENDERABLE_STATIC3D].allocComponent(target);
        *pRenderable = Static3DRenderable(meshID);
        addComponent(target, pRenderable);
        return pRenderable;
    }

    SkinnedRenderable* Scene::createSkinnedRenderable(
        entityID_t target,
        PK_id modelID,
        PK_id meshID
    )
    {
        if ((getEntity(target).componentMask & ComponentType::PK_TRANSFORM) != ComponentType::PK_TRANSFORM)
        {
            Debug::log(
                "@Scene::createSkinnedRenderable "
                "Created renderable component for entity: " + std::to_string(target) + " "
                "with no Transform component! This prevents rendering if transform not specified!",
                Debug::MessageType::PK_WARNING
            );
        }

        SkinnedRenderable* pRenderable = (SkinnedRenderable*)componentPools[ComponentType::PK_RENDERABLE_SKINNED].allocComponent(target);
        *pRenderable = SkinnedRenderable(modelID, meshID);
        addComponent(target, pRenderable);
        return pRenderable;
    }

    TerrainRenderable* Scene::createTerrainRenderable(
        entityID_t target,
        PK_id terrainMeshID,
        PK_id terrainMaterialID,
        const std::vector<float>& heightmap,
        float tileWidth
    )
    {
        if ((getEntity(target).componentMask & ComponentType::PK_TRANSFORM) != ComponentType::PK_TRANSFORM)
        {
            Debug::log(
                "@Scene::createTerrainRenderable "
                "Created terrain renderable component for entity: " + std::to_string(target) + " "
                "with no Transform component! This prevents rendering if transform not specified!",
                Debug::MessageType::PK_WARNING
            );
        }

        TerrainRenderable* pRenderable = (TerrainRenderable*)componentPools[ComponentType::PK_RENDERABLE_TERRAIN].allocComponent(target);
        *pRenderable = TerrainRenderable(
            terrainMeshID,
            terrainMaterialID,
            heightmap,
            tileWidth
        );
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
        const float farPlane2D = (float)(UIRenderableComponent::get_max_layers() + 1);
        const float nearPlane3D = 0.1f;
        mat4 orthographicProjMat = create_proj_mat_ortho(0, windowWidth, windowHeight, 0, 0.0f, farPlane2D);
        mat4 perspectivaProjMat = create_perspective_projection_matrix(aspectRatio, 1.3f, nearPlane3D, 1000.0f);

        Camera* pCamera = (Camera*)componentPools[ComponentType::PK_CAMERA].allocComponent(target);
        *pCamera = Camera(orthographicProjMat, perspectivaProjMat, nearPlane3D);
        addComponent(target, pCamera);
        Transform::create(target, position, { 1, 1, 1 }, pitch, yaw);

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

    AnimationData* Scene::createAnimationData(
        entityID_t target,
        PK_id animationResourceID,
        AnimationMode mode,
        float speed,
        std::vector<uint32_t> keyframes
    )
    {
        AnimationData* pComponent = (AnimationData*)componentPools[ComponentType::PK_ANIMATION_DATA].allocComponent(target);
        // For now figure out joint count here.. maybe in future somewhere else..
        ResourceManager& resManager = Application::get()->getResourceManager();
        const Animation* pAnimResource = (const Animation*)resManager.getResource(animationResourceID);
        if (!pAnimResource)
        {
            Debug::log(
                "@Scene::createAnimationData "
                "Failed to find AnimationResource with ID: " + std::to_string(animationResourceID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        *pComponent = AnimationData(
            animationResourceID,
            mode,
            speed,
            pAnimResource->getBindPose(),
            keyframes
        );
        addComponent(target, pComponent);
        return pComponent;
    }

    Blinker* Scene::createBlinker(entityID_t target)
    {
        Blinker* pComponent = (Blinker*)componentPools[ComponentType::PK_BLINKER].allocComponent(target);
        Blinker b;
        *pComponent = b;
        addComponent(target, pComponent);
        return pComponent;
    }

    Component* Scene::getComponent(
        entityID_t entityID,
        ComponentType type,
        bool nestedSearch,
        bool enableWarning
    )
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
        if ((entities[entityID].componentMask & (uint64_t)type) == (uint64_t)type)
        {
            return (Component*)componentPools[type].getComponent_DANGER(entityID);
        }
        if (!nestedSearch && enableWarning)
            Debug::log(
                "Couldn't find component of type: " + std::to_string(type) + " "
                "from entity: " + std::to_string(entityID),
                Debug::MessageType::PK_WARNING
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
            if ((entity.componentMask & it->first) == it->first)
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
    std::vector<Component*> Scene::getAllComponents(entityID_t entity)
    {
        std::vector<Component*> ownComponents = getComponents(entity);
        for (uint32_t e : entityChildMapping[entity])
        {
            std::vector<Component*> childComponents = getAllComponents(e);
            for (Component* c : childComponents)
                ownComponents.push_back(c);
        }
        return ownComponents;
    }

    // Returns all components in scene of specific type
    //std::vector<Component*> Scene::getComponentsOfTypeInScene(ComponentType type)
    //{
    //    std::vector<Component*> foundComponents;
    //    for (uint32_t componentID : typeComponentMapping[type])
    //        foundComponents.push_back(components[componentID]);
    //    return foundComponents;
    //}
}
