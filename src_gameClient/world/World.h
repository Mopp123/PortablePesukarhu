#pragma once

#include <cstdint>
#include <vector>

#include "../../pk/core/Scene.h"

#include "../../pk/ecs/components/renderable/TerrainTileRenderable.h"
#include "../net/requests/Request.h"
#include "../../pk/Common.h"

namespace world
{

	struct WorldObserver
	{
		int observeRadius = 5;
		int mapX = 0;
		int mapY = 0;
	};


	//	Visual representation of server's "world state" at currently observed coordinates
	class VisualWorld
	{
	private:

		
		class OnCompletion_fetchWorldState : public net::OnCompletionEvent
		{
		public:
			VisualWorld& visualWorldRef;
			OnCompletion_fetchWorldState(world::VisualWorld& visualWorld) : visualWorldRef(visualWorld) {}
			virtual void func(const PK_byte* data, size_t dataSize);
		};


		pk::Scene& _sceneRef;
		std::vector<pk::TerrainTileRenderable*> _tileRenderables;
		float _tileVisualScale = 2.0f;

		WorldObserver _observer;
		
		float _maxUpdateCooldown = 0.25f;
		float _updateCooldown = 0.0f;


		float _worldX, _worldZ;

	public:

		VisualWorld(pk::Scene& scene);
		~VisualWorld();

		void updateTileVisuals(const uint64_t* mapState);

		void update(float worldX, float worldZ);
	};

}