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

	float get_distance(const vec3& left, const vec3& right);

	bool rayTriangleIntersect_MOLLER_TRUMBORE(
	    const vec3 &orig, const vec3 &dir,
	    const vec3 &v0, const vec3 &v1, const vec3 &v2,
	    float &t, float &u, float &v
	);

	bool rayPolygonIntersect(
	    const vec3 &orig,
		const vec3 &dir,
		const vec3& colliderPos,
		const vec3& colliderScale,
	    float &t, float &u, float &v
	);
}
