#pragma once

#include "pesukarhu/ecs/components/Component.h"
#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/utils/pkmath.h"


namespace pk
{
	// This isn't supposed to be used by itself but as a base for different kinds of lights!
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

		DirectionalLight(const vec3& color, const vec3& direction);
		~DirectionalLight();

		static DirectionalLight* create(
            entityID_t target,
            const vec3& color,
            const vec3& direction
		);
	};
}
