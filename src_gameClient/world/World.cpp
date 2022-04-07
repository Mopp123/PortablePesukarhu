
#include "World.h"
#include "Tile.h"
#include "../net/Client.h"
#include "../net/requests/Commands.h"
#include "../net/NetCommon.h"

#include "../../pk/core/Application.h"
#include "../../pk/core/Timing.h"

#include <algorithm>

using namespace pk;
using namespace net;
using namespace net::web;

namespace world
{



	void VisualWorld::OnCompletion_fetchWorldState::func(const PK_byte* data, size_t dataSize)
	{
		const int dataWidth = (5 * 2) + 1;
		const size_t expectedDataSize = (dataWidth * dataWidth) * sizeof(uint64_t);

		// Attempt to print fetched area
		if (dataSize >= expectedDataSize)
		{
			const uint64_t* dataBuf = (const uint64_t*)data;
			visualWorldRef.updateTileVisuals(dataBuf);
		}
	}


	VisualWorld::VisualWorld(pk::Scene& scene) : 
		_sceneRef(scene)
	{
		// Create visual tiles at first as "blank" -> we configure these eventually, when we fetch world state from server
		const int observeAreaWidth = _observer.observeRadius * 2 + 1;
		for (int y = 0; y < observeAreaWidth; ++y)
		{
			for (int x = 0; x < observeAreaWidth; ++x)
			{
				uint32_t tileEntity = _sceneRef.createEntity();
				TerrainTileRenderable* tileRenderable = new TerrainTileRenderable(x * _tileVisualScale, y * _tileVisualScale, _tileVisualScale);
				_sceneRef.addComponent(tileEntity, tileRenderable);
				_tileRenderables.push_back(tileRenderable);
			}
		}
	}

	VisualWorld::~VisualWorld()
	{}

	// ..quite shit and inefficient
	void VisualWorld::updateTileVisuals(const uint64_t* mapState)
	{
		const int observeAreaWidth = _observer.observeRadius * 2 + 1;

		// this is used to group together all tiles' vertices which share the same pos
		std::unordered_map<int, float> sharedVertexHeights;
		
		// Update shared heights mapping
		for (int y = 0; y < observeAreaWidth; ++y)
		{
			for (int x = 0; x < observeAreaWidth; ++x)
			{
				uint64_t tile = mapState[x + y * observeAreaWidth];

				_tileRenderables[x + y * observeAreaWidth]->tileMapX = (_observer.mapX + x) * _tileVisualScale;
				_tileRenderables[x + y * observeAreaWidth]->tileMapY = (_observer.mapY + y) * _tileVisualScale;

				PK_ubyte terrInfo = get_tile_terrinfo(tile);

				for (int j = 0; j < 2; ++j)
				{
					for (int i = 0; i < 2; ++i)
					{
						int sharedHeightIndex = (x + i) + (y + j) * observeAreaWidth;
						float height = (float)terrInfo;
						sharedVertexHeights[sharedHeightIndex] = std::max(sharedVertexHeights[sharedHeightIndex], height);
					}
				}
			}
		}

		// Tiles' vertices' heights' combining/smoothing according to "sharedVertexHeights"-mapping
		for (int y = 0; y < observeAreaWidth; ++y)
		{
			for (int x = 0; x < observeAreaWidth; ++x)
			{
				int tileIndex = x + y * observeAreaWidth;
				TerrainTileRenderable* t = _tileRenderables[tileIndex];

				for (int j = 0; j < 2; ++j)
				{
					for (int i = 0; i < 2; ++i)
					{
						if ((x == observeAreaWidth - 1 && i == 1) || (x == 0 && i == 0)) // This looks fucking disqusting, but for now it prevents "heightbleedingbugthing"
							continue;

						int heightIndex = i + j * 2;
						int sharedVertexIndex = (x + i) + (y + j) * observeAreaWidth;
						_tileRenderables[tileIndex]->vertexHeights[heightIndex] = sharedVertexHeights[sharedVertexIndex];
					}
				}
			}
		}
	}


	void VisualWorld::update(float worldX, float worldZ)
	{
		// JUST FOR TESTING
		float offset = -11;

		_worldX = worldX;
		_worldZ = worldZ;
		
		// Calc the "real map pos" according to "visual float pos"(this should be camera's pivot point, if rts style camera)
		_observer.mapX = (int)std::floor((_worldX + offset) / _tileVisualScale);
		_observer.mapY = (int)std::floor((_worldZ + offset * 2) / _tileVisualScale);

		if (_updateCooldown <= 0.0f)
		{
			send_command(Client::get_instance()->getUserID(), CMD_FetchWorldState, _observer.mapX, _observer.mapY, new OnCompletion_fetchWorldState(*this));
			_updateCooldown = _maxUpdateCooldown;
		}
		else
		{
			_updateCooldown -= 1.0f * Timing::get_delta_time();
		}
	}
}