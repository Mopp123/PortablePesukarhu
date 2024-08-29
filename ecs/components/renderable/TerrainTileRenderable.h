#pragma once

#include "ecs/components/Component.h"
#include <cstdint>
#include "utils/pkmath.h"
#include "resources/Texture.h"
#include <vector>

#define PK_TERRAINTILE_HEIGHT_INDEX_TOP_LEFT		0
#define PK_TERRAINTILE_HEIGHT_INDEX_BOTTOM_LEFT		1
#define PK_TERRAINTILE_HEIGHT_INDEX_BOTTOM_RIGHT	2
#define PK_TERRAINTILE_HEIGHT_INDEX_TOP_RIGHT		3


namespace pk
{

	class TerrainTileRenderable : public Component
	{
	public:

		static Texture* s_blendmapTexture;
		static Texture* s_channelTexture0;
		static Texture* s_channelTexture1;
		static Texture* s_channelTexture2;
		static Texture* s_channelTexture3;
		static Texture* s_channelTexture4;
		static int s_gridWidth;

		float vertexHeights[4]; // 0 = "top left"	3 = "top right"
		vec3 vertexNormals[4];
		vec2 textureOffset;
		int32_t worldX = 0;
		int32_t worldZ = 0;
		// These grid coords means what is the position of this specific tile in the "tile grid"
		int gridX = 0;
		int gridY = 0;

		float scale = 10.0f;

		TerrainTileRenderable(int32_t worldX, int32_t worldZ, int gridX, int gridY, float scale);

		inline float getAverageHeight() const { return (vertexHeights[0] + vertexHeights[1] + vertexHeights[2] + vertexHeights[3]) / 4; }
	};
}
