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

		// Coordinates where we requested our latest "world state update" (not immediately received from server!)
		int requestedMapX = 0;
		int requestedMapY = 0;

		// Coordinates where we got the latest "world state update" successfully
		int lastReceivedMapX = 0;
		int lastReceivedMapY = 0;
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
		
		float _maxUpdateCooldown = 1.0f;
		float _updateCooldown = 0.0f;

	public:

		VisualWorld(pk::Scene& scene, int observeRadius);
		~VisualWorld();

		void updateTileVisuals(const uint64_t* mapState);

		void update(float worldX, float worldZ);

		float getTileVisualHeightAt(float worldX, float worldZ) const;

		pk::vec3 getMousePickCoords(const pk::mat4& projMat, const pk::mat4& viewMat) const;
	};

}