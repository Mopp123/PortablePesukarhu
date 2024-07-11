#include "TerrainTileRenderable.h"
#include <cstring>


namespace pk
{
	Texture* TerrainTileRenderable::s_blendmapTexture = nullptr;
	Texture* TerrainTileRenderable::s_channelTexture0 = nullptr;
	Texture* TerrainTileRenderable::s_channelTexture1 = nullptr;
	Texture* TerrainTileRenderable::s_channelTexture2 = nullptr;
	Texture* TerrainTileRenderable::s_channelTexture3 = nullptr;
	Texture* TerrainTileRenderable::s_channelTexture4 = nullptr;
	int TerrainTileRenderable::s_gridWidth = 20 * 20 + 1;

	TerrainTileRenderable::TerrainTileRenderable(int32_t worldX, int32_t worldZ, int gridX, int gridY, float scale) :
		Component(ComponentType::PK_RENDERABLE_TERRAINTILE),
		worldX(worldX), worldZ(worldZ), gridX(gridX), gridY(gridY), scale(scale)
	{
		memset(vertexHeights, 0, sizeof(float) * 4);
		memset(vertexNormals, 0, sizeof(vec3) * 4);

	}
}
