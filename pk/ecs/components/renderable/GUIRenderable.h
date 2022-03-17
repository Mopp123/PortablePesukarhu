#pragma once

#include "../Component.h"
#include "../../../utils/pkmath.h"

namespace pk
{

	class GUIRenderable : public Component
	{
	public:

		vec2 pos;
		vec2 scale;
		int textureID = 0;

		GUIRenderable() :
			Component(ComponentType::PK_RENDERABLE_GUI)
		{}

	};
}