#pragma once

#include "../Component.h"
#include "../../../utils/pkmath.h"

namespace pk
{

	class Light : public Component
	{
	public:

		vec3 color;

		Light(const vec3& color, ComponentType lightType) :
			Component(lightType),
			color(color)
		{}

		virtual ~Light(){}
	};


	class DirectionalLight : public Light
	{
	public:
		vec3 direction;

		DirectionalLight(const vec3& color, const vec3& direction) :
			Light(color, ComponentType::PK_LIGHT_DIRECTIONAL),
			direction(direction)
		{
		}

		~DirectionalLight() {}
	};
}