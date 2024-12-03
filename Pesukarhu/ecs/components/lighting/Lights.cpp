#include "Lights.h"
#include "Pesukarhu/core/Application.h"


namespace pk
{

	DirectionalLight::DirectionalLight(const vec3& color, const vec3& direction) :
		Light(color, ComponentType::PK_LIGHT_DIRECTIONAL),
		direction(direction)
	{
	}

	DirectionalLight::~DirectionalLight()
	{}

	DirectionalLight* DirectionalLight::create(
        entityID_t target,
        const vec3& color,
        const vec3& direction
	)
	{
		Scene* pScene = Application::get()->accessCurrentScene();
        DirectionalLight* pDirectionalLight = (DirectionalLight*)pScene->componentPools[ComponentType::PK_LIGHT_DIRECTIONAL].allocComponent(target);
        *pDirectionalLight = DirectionalLight(color, direction);
        pScene->addComponent(target, pDirectionalLight);
        return pDirectionalLight;
	}
}
