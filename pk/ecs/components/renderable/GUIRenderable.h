#pragma once

#include "../Component.h"

namespace pk
{

	class GUIRenderable : public Component
	{
	public:

		GUIRenderable() :
			Component(ComponentType::PK_RENDERABLE_GUI)
		{
		}

	};
}