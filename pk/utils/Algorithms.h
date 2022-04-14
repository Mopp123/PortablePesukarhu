#pragma once

#include "pkmath.h"

namespace pk
{
	
	float lerp(float a, float b, float amount);

	vec2 screen_to_ndc(int screenX, int screenY);

	// Converts screen coordinate to "3d world coordinate"
	vec3 screen_to_world_space(int screenX, int screenY, const mat4& projMat, const mat4& viewMat);

	// Returns next closest power of 2 value from v
	unsigned int get_next_pow2(unsigned int v);

}