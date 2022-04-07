#pragma once


#include "../../pk/core/Scene.h"

#include "../../pk/ecs/systems/ui/Text.h"

#include "../../pk/ecs/components/renderable/TerrainTileRenderable.h"
#include "../../pk/ecs/systems/CameraUtils.h"

#include "../world/World.h"

#include <vector>

class InGame : public pk::Scene
{
private:

	pk::ui::Text* _pText_debug_delta = nullptr;

	world::VisualWorld* _visualWorld = nullptr;

	pk::CameraController* _pCamController = nullptr;

	pk::mat4* _pCamTransform = nullptr;

public:

	InGame();
	~InGame();

	virtual void init();
	virtual void update();

};