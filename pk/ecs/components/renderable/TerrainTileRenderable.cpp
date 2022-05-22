
#include "TerrainTileRenderable.h"

namespace pk
{
	Texture* TerrainTileRenderable::s_blendmapTexture = nullptr;
	int TerrainTileRenderable::s_gridWidth = 20 * 20 + 1;

	TerrainTileRenderable::TerrainTileRenderable(int32_t worldX, int32_t worldZ, int gridX, int gridY, float scale) :
		Component(ComponentType::PK_RENDERABLE_TERRAINTILE),
		worldX(worldX), worldZ(worldZ), gridX(gridX), gridY(gridY), scale(scale)
	{
		memset(vertexHeights, 0, sizeof(float) * 4);
		memset(vertexNormals, 0, sizeof(vec3) * 4);
	}
}