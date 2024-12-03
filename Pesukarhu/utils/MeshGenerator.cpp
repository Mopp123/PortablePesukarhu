#include "MeshGenerator.h"
#include "pkmath.h"


namespace pk
{
	std::pair<Buffer*, Buffer*> generate_terrain_mesh_data(
		const std::vector<float>& heightmap,
		float tileWidth,
		BufferUpdateFrequency updateFrequency
	)
	{
		// expecting heightmap always to be square
		uint32_t verticesPerRow = sqrt(heightmap.size());

		const size_t vertexComponentCount = 3 + 3 + 2;
		std::vector<vec3> vertexPositions;
		std::vector<vec3> vertexNormals;
		std::vector<vec2> vertexTexCoords;

		uint32_t vertexCount = 0;

		// actual "world space width"
		float totalWidth = tileWidth * verticesPerRow;

		for (int z = 0; z < verticesPerRow; z++)
		{
			for (int x = 0; x < verticesPerRow; x++)
			{
				vec3 vertexPos(x * tileWidth, heightmap[x + z * verticesPerRow], z * tileWidth);
				vertexPositions.push_back(vertexPos);

				// Figure out normal (quite shit and unpercise way, but it looks fine for now..)
				// Calc normal
				float left = 0;
				float right = 0;
				float down = 0;
				float up = 0;

				int heightmapX = x;
				int heightmapY = z;

				if (heightmapX - 1 >= 0)
					left = heightmap[(heightmapX - 1) + heightmapY * verticesPerRow];

				if (heightmapX + 1 < verticesPerRow)
					right = heightmap[(heightmapX + 1) + heightmapY * verticesPerRow];

				if (heightmapY + 1 < verticesPerRow)
					up = heightmap[heightmapX + (heightmapY + 1) * verticesPerRow];

				if (heightmapY - 1 >= 0)
					down = heightmap[heightmapX + (heightmapY - 1) * verticesPerRow];


				//float heightVal = heightmap[x + z * verticesPerRow];
				//vec3 normal((left - right), _heightModifier * 0.1f, (down - up)); // this is fucking dumb...
				vec3 normal((left - right), 1.0f, (down - up)); // this is fucking dumb...
				normal = normal.normalize();
				vertexNormals.push_back(normal);

				// uv
				vec2 texCoord(vertexPos.x / totalWidth, vertexPos.z / totalWidth);
				vertexTexCoords.push_back(texCoord);

				vertexCount++;
			}
		}

		// Combine all into 1 buffer
		std::vector<float> vertexData;
		vertexData.reserve(vertexComponentCount * vertexCount);
		for (int i = 0; i < vertexCount; i++)
		{
			vertexData.emplace_back(vertexPositions[i].x);
			vertexData.emplace_back(vertexPositions[i].y);
			vertexData.emplace_back(vertexPositions[i].z);

			vertexData.emplace_back(vertexNormals[i].x);
			vertexData.emplace_back(vertexNormals[i].y);
			vertexData.emplace_back(vertexNormals[i].z);

			vertexData.emplace_back(vertexTexCoords[i].x);
			vertexData.emplace_back(vertexTexCoords[i].y);
		}

		std::vector<uint32_t> indices;
		for (uint32_t x = 0; x < verticesPerRow; x++)
		{
			for (uint32_t z = 0; z < verticesPerRow; z++)
			{
				if (x >= verticesPerRow - 1 || z >= verticesPerRow - 1)
					continue;

				uint32_t topLeft = x + (z + 1) * verticesPerRow;
				uint32_t bottomLeft = x + z * verticesPerRow;

				uint32_t topRight = (x + 1) + (z + 1) * verticesPerRow;
				uint32_t bottomRight = (x + 1) + z * verticesPerRow;


				indices.push_back(bottomLeft);
				indices.push_back(topLeft);
				indices.push_back(topRight);

				indices.push_back(topRight);
				indices.push_back(bottomRight);
				indices.push_back(bottomLeft);
			}
		}

		// Buffer for each swapchain img since we expect that the vertex buffer
		// may change due to altered heightmap!
		Buffer* pVertexBuffer = Buffer::create(
			vertexData.data(),
			sizeof(float),
			vertexData.size(),
			BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT,
			updateFrequency,
			true // If we want to alter heights at runtime
		);
		Buffer* pIndexBuffer = Buffer::create(
			indices.data(),
			sizeof(uint32_t),
			indices.size(),
			BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT,
			BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STATIC
		);

		return std::make_pair(pVertexBuffer, pIndexBuffer);
	}
}
