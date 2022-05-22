#pragma once


#include "../../pk/core/Scene.h"


#include "../../pk/ecs/components/renderable/Sprite3DRenderable.h"
#include "../../pk/ecs/components/renderable/TerrainTileRenderable.h"

#include "../../pk/ecs/systems/ui/combinedFunctional/InputField.h"

#include "../../pk/ecs/systems/ui/Text.h"
#include "../../pk/ecs/systems/CameraUtils.h"
#include "../../pk/ecs/systems/animations/SpriteAnimations.h"

#include "../world/World.h"

#include <vector>

class InGame : public pk::Scene
{
private:

	pk::ui::Text* _pText_debug_delta = nullptr;

	world::VisualWorld* _visualWorld = nullptr;

	pk::Sprite3DRenderable* _testSprite = nullptr;
	pk::SpriteAnimator* _spriteAnimator = nullptr;

	// just testing tile animating..
	pk::SpriteAnimator* _animator_water = nullptr;
	pk::SpriteAnimator* _animator_grass = nullptr;

	pk::RTSCamController* _pCamController = nullptr;
	pk::mat4* _pCamTransform = nullptr;


	pk::ui::InputField* _inputField_position = nullptr;

public:

	InGame();
	~InGame();

	virtual void init();
	virtual void update();

};