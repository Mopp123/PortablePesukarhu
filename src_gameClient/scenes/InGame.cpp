
#include "InGame.h"

#include "../net/Client.h"

#include "../net/Client.h"
#include "../net/requests/Commands.h"
#include "../net/NetCommon.h"

#include "../../pk/ecs/components/lighting/Lights.h"

#include "../../pk/core/Debug.h"
#include <iostream>
#include <chrono>

using namespace pk;
using namespace ui;
using namespace net;
using namespace net::web;


InGame::InGame()
{}


InGame::~InGame()
{
	delete _pCamController;

	delete _visualWorld;

	delete _pText_debug_delta;
}




static std::string s_TEST_worldstate;
static Text* s_TEST_text = nullptr;

void InGame::init()
{
	// create default camera
	activeCamera = create_camera({ 0,0,0 });
	// create dir light
	uint32_t lightEntity = createEntity();
	vec3 lightDir(0.0f, -0.5f, -0.5f);
	lightDir.normalize();
	addComponent(lightEntity, new DirectionalLight({ 1,1,1 }, lightDir));



	const float textSize = 16;
	const float rowPadding = 5;

	const float buttonSize = 24;

	const float panelX = 128;
	const float panelY = 128;

	// For debugging
	_pText_debug_delta = new Text(
		"",
		{
			{ ConstraintType::PIXEL_TOP, 5 },
			{ ConstraintType::PIXEL_LEFT, 5 }
		}
	);


	std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::high_resolution_clock::now();
	
	Client::get_instance()->setUserID("Persekorva666");
	_visualWorld = new world::VisualWorld(*this, 10);

	float delta = (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - startTime)).count();
	
	Debug::log("building took: " + std::to_string(delta));

	_pCamTransform = &((Transform*)getComponent(activeCamera->getEntity(), ComponentType::PK_TRANSFORM))->accessTransformationMatrix();
	_pCamTransform->setIdentity();
	mat4& camTransform = *_pCamTransform;

	camTransform[0 + 3 * 4] = 10.0f;
	camTransform[1 + 3 * 4] = 10.0f;
	camTransform[2 + 3 * 4] = 25.0f;

	float angle = -0.7f;

	camTransform[1 + 1 * 4] = cos(angle);
	camTransform[1 + 2 * 4] = -sin(angle);
	
	camTransform[2 + 1 * 4] = sin(angle);
	camTransform[2 + 2 * 4] = cos(angle);


	_pCamController = new RTSCamController(*activeCamera, this);


	// TESTING 3D sprites..
	uint32_t spriteEntity = createEntity();
	_testSprite = new Sprite3DRenderable({ 0,0,0 }, { 5,5 });
	addComponent(spriteEntity, _testSprite);
}

void InGame::update()
{
	vec3 camPivotPoint = _pCamController->getPivotPoint();
	
	// attempt to glue cam's height to terrain's height
	camPivotPoint.y = _visualWorld->getTileVisualHeightAt(camPivotPoint.x, camPivotPoint.z);
	_pCamController->setPivotPoint(camPivotPoint);
	
	_visualWorld->update(camPivotPoint.x, camPivotPoint.z);
	
	

	_pText_debug_delta->accessRenderable()->accessStr() = "Delta: " + std::to_string(Timing::get_delta_time());


	// debug mouse picking testing..
	mat4 viewMatrix = *_pCamTransform;
	viewMatrix.inverse();

	_testSprite->position = vec3(22, 5, 22);//_visualWorld->getMousePickCoords(activeCamera->getProjMat3D(), viewMatrix);
}