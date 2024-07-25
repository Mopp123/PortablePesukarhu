﻿#pragma once

#include "ecs/components/ComponentPool.h"
#include "ecs/Entity.h"

#include "ecs/components/Component.h"
#include "ecs/components/Camera.h"
#include "ecs/components/ui/ConstraintData.h"
#include "ecs/components/ui/UIElemState.h"
#include "ecs/components/renderable/GUIRenderable.h"
#include "ecs/components/renderable/TextRenderable.h"
#include "ecs/components/renderable/Static3DRenderable.h"
#include "ecs/components/lighting/Lights.h"
#include "graphics/Environment.h"

#include "../ecs/systems/System.h"
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

        // TODO: delete below?
        //std::unordered_map<uint32_t, Component*> components;

        // TODO: delete below
        //std::unordered_map<ComponentType, std::vector<uint32_t>> typeComponentMapping;

        entityID_t activeCamera = NULL_ENTITY_ID;
        entityID_t directionalLight = NULL_ENTITY_ID;
        struct EnvironmentProperties environmentProperties;

        Scene();
        virtual ~Scene();

        entityID_t createEntity();
        void destroyEntity(entityID_t entityID);
        void addChild(entityID_t entityID, entityID_t childID);
        std::vector<entityID_t> getChildren(entityID_t entityID);

        void addComponent(entityID_t entityID, Component* component);
        inline bool isValidEntity(entityID_t entityID) const
        {
            if (entityID < 0 || entityID >= entities.size())
                return false;
            return entities[entityID].id != NULL_ENTITY_ID;
        }
        Transform* createTransform(entityID_t target, vec2 pos, vec2 scale);
        Transform* createTransform(
            entityID_t target,
            vec3 pos,
            vec3 scale,
            float pitch = 0.0f,
            float yaw = 0.0f
        );
        ConstraintData* createUIConstraint(
            entityID_t target,
            HorizontalConstraintType horizontalType,
            float horizontalValue,
            VerticalConstraintType verticalType,
            float verticalValue
        );
        UIElemState* createUIElemState(entityID_t target);
        GUIRenderable* createGUIRenderable(
            entityID_t target,
            Texture_new* pTexture = nullptr,
            vec3 color = { 1, 1, 1 },
            vec4 borderColor = { 1, 1, 1, 1 },
            float borderThickness = 0.0f,
            vec4 textureCropping = vec4(0, 0, 1, 1)
        );
        TextRenderable* createTextRenderable(
            entityID_t target,
            const std::string& txt,
            PK_id fontID,
            vec3 color,
            bool bold = false
        );
        Static3DRenderable* createStatic3DRenderable(
            entityID_t target,
            PK_id meshID
        );
        Camera* createCamera(
            entityID_t target,
            const vec3& position,
            float pitch,
            float yaw
        );
        DirectionalLight* createDirectionalLight(
            entityID_t target,
            const vec3& color,
            const vec3& direction
        );

        // TODO: all getComponent things could be optimized?
        Component* getComponent(entityID_t entityID, ComponentType type, bool nestedSearch = false);
        const Component * const getComponent(entityID_t entityID, ComponentType type, bool nestedSearch = false) const;
        // Returns first component of "type" found in "entity"'s child entities
        Component* getComponentInChildren(entityID_t entityID, ComponentType type);

        // Return all components of entity
        std::vector<Component*> getComponents(entityID_t entityID);

        // Returns first found component of type "type"
        //Component* getComponent(ComponentType type);

        //std::vector<Component*> getComponentsInChildren(uint32_t entity);

        // Returns all components of entity and its' children all the way down the hierarchy
        //std::vector<Component*> getAllComponents(uint32_t entity);

        // Returns all components in scene of specific type
        //std::vector<Component*> getComponentsOfTypeInScene(ComponentType type);

        entityID_t getActiveCamera() const { return activeCamera; }

        virtual void init() = 0;
        virtual void update() = 0;
    };
}
