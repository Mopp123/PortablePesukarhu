#pragma once

#include "../Component.h"
#include <cstdint>
#include "../../../utils/pkmath.h"


#define PK_TERRAINTILE_HEIGHT_INDEX_TOP_LEFT		0
#define PK_TERRAINTILE_HEIGHT_INDEX_BOTTOM_LEFT		1
#define PK_TERRAINTILE_HEIGHT_INDEX_BOTTOM_RIGHT	2
#define PK_TERRAINTILE_HEIGHT_INDEX_TOP_RIGHT		3


namespace pk
{

	class TerrainTileRenderable : public Component
	{
	public:

		float vertexHeights[4]; // 0 = "top left"	3 = "top right"
		vec3 vertexNormals[4];

		int32_t tileMapX = 0;
		int32_t tileMapY = 0; // in 3d this is the actual world z coord (tuo kartta meinaa tässä 2d projektiota 3d maailmasta)
		float scale = 10.0f;
		vec3 color;

		TerrainTileRenderable(int32_t mapX, int32_t mapY, float scale) :
			Component(ComponentType::PK_RENDERABLE_TERRAINTILE),
			tileMapX(mapX), tileMapY(mapY), scale(scale), color(1, 1, 1)
		{
			memset(vertexHeights, 0, sizeof(float) * 4);
			memset(vertexNormals, 0, sizeof(vec3) * 4);
		}

	};
}