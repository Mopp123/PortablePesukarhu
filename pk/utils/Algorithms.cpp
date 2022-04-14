

#include "Algorithms.h"
#include "../core/Application.h"

#include <iostream>


namespace pk
{
	float lerp(float a, float b, float amount)
	{
		return a + amount * (b - a);
	}


	vec2 screen_to_ndc(int screenX, int screenY)
	{
		const Window* const window = Application::get()->getWindow();
		
		return { ((float)screenX / (float)window->getWidth()) * 2.0f - 1.0f,
				((float)screenY / (float)window->getHeight()) * 2.0f - 1.0f };
	}


	// Converts screen coordinate to "3d world coordinate"
	vec3 screen_to_world_space(int screenX, int screenY, const mat4& projMat, const mat4& viewMat)
	{
		const Window* const window = Application::get()->getWindow();

		float ndc_x = ((float)screenX / (float)window->getWidth()) * 2.0f - 1.0f;
		float ndc_y = ((float)screenY / (float)window->getHeight()) * 2.0f - 1.0f;

		vec4 clipCoords(ndc_x, ndc_y, -1.0f, 1.0f);

		mat4 invProjMat = projMat;
		invProjMat.inverse();

		vec4 cameraSpace = invProjMat * clipCoords;
		cameraSpace.z = -1.0f;
		cameraSpace.w = 0.0f;

		mat4 invViewMat = viewMat;
		invViewMat.inverse();

		vec4 worldSpace = invViewMat * cameraSpace;
		worldSpace.w = 0.0f;
		worldSpace.normalize();
		
		return { worldSpace.x, worldSpace.y ,worldSpace.z };
	}


	// Returns next closest power of 2 value from v
	unsigned int get_next_pow2(unsigned int v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

}