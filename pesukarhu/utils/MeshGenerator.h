#pragma once

#include "pesukarhu/graphics/Buffers.h"
#include <vector>
#include <utility>


namespace pk
{
    // NOTE: Vertex data is generated from 0,0,0 towards +z direction
	// first = vertex buffer, second = index buffer
	std::pair<Buffer*, Buffer*> generate_terrain_mesh_data(
		const std::vector<float>& heightmap,
		float tileWidth,
		BufferUpdateFrequency updateFrequency = BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STATIC
	);
}
