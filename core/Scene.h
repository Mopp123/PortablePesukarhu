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
				for (Component* c : cContainer.second)
					delete c;
			}
			components.clear();
			entities.clear();
		}

		uint32_t createEntity()
		{
			// *Start entity ids from 1 and NOT from 0
			uint32_t id = entities.size() + 1;
			entities.push_back(id);
			return id;
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
		Component* getComponent(uint32_t entity, ComponentType type)
		{
			for (Component* c : components[type])
			{
				if (c->getEntity() == entity)
					return c;
			}
			return nullptr;
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

		virtual void init() = 0;
		virtual void update() = 0;

	};	
}
