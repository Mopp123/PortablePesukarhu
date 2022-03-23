#pragma once

#include "UIElement.h"
#include "../../components/renderable/GUIRenderable.h"

namespace pk
{
	namespace ui
	{
		class GUIImage : public UIElement
		{
		private:

			uint32_t _id = 0;

			GUIRenderable* _renderable = nullptr;

		public:

			// *Give width and height in pixels(even they are floats here...)
			GUIImage(const std::vector<Constraint>& constraints, float width, float height);
			~GUIImage();

			inline GUIRenderable* accessRenderable() { return _renderable; }
		};
	}
}