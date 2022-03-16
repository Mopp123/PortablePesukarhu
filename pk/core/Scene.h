#pragma once

#include "../ecs/components/Component.h"
#include <unordered_map>
#include <vector>

namespace pk
{

	class Scene
	{
	public:

		std::unordered_map<ComponentType, std::vector<Component*>> components;

		Scene() {}
		virtual ~Scene() 
		{
			for (const std::pair<ComponentType, std::vector<Component*>>& cContainer : components)
			{
				for (Component* c : cContainer.second)
					delete c;
			}
			components.clear();
		}

		virtual void init() = 0;
		virtual void update() = 0;
	};	
}