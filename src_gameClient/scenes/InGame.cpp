
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

/*
class OnCompletion_fetchWorldState : public OnCompletionEvent
{
public:

	world::VisualWorld& visualWorldRef;
	OnCompletion_fetchWorldState(world::VisualWorld& visualWorld) : visualWorldRef(visualWorld) {}

	virtual void func(const std::vector<ByteBuffer>& data)
	{
		Debug::log("Req completed!");

		const int dataWidth = (5 * 2) + 1;
		const size_t expectedDataSize = (dataWidth * dataWidth) * sizeof(uint64_t);
		
		const size_t acquiredSize = (const size_t)data[0].getSize();

		//Debug::log("size was: " + std::to_string(acquiredSize) + " Expected: " + std::to_string(expectedDataSize));

		// Attempt to print fetched area
		if (acquiredSize >= expectedDataSize)
		{
			const uint64_t* dataBuf = (const uint64_t*)data[0].getRawData();	
			visualWorldRef.updateTileVisuals(dataBuf);
		}
	}
};
*/

//static int s_TEST_xPos = 0;
//static int s_TEST_zPos = 0;

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
	_visualWorld = new world::VisualWorld(*this);

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


	_pCamController = new CameraController(*activeCamera, this);
}

void InGame::update()
{
	mat4& camTransform = *_pCamTransform;

	float cx = camTransform[0 + 3 * 4];
	float cy = camTransform[1 + 3 * 4];
	float cz = camTransform[2 + 3 * 4];

	Debug::log("cx: " + std::to_string(cx) + " cy: " + std::to_string(cy) + " cz: " + std::to_string(cz));

	_visualWorld->update(cx, cz);

	_pText_debug_delta->accessRenderable()->accessStr() = "Delta: " + std::to_string(Timing::get_delta_time());

}