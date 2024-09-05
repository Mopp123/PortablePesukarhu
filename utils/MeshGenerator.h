#pragma once

#include "resources/Mesh.h"
#include <vector>
#include <utility>


namespace pk
{
    // NOTE: Vertex data is generated from 0,0,0 towards +z direction
	// first = vertex buffer second = index buffer
	std::pair<Buffer*, Buffer*> generate_terrain_mesh_data(const std::vector<float>& heightmap, float tileWidth);
}
