#pragma once

#include "UIElement.h"
#include "../../components/renderable/TextRenderable.h"

namespace pk
{
	namespace ui
	{
		class Text : public UIElement
		{
		private:

			uint32_t _id = 0;
			TextRenderable* _renderable = nullptr;

		public:

			Text(const std::string& str, const std::vector<Constraint>& constraints, bool bold = false);
			Text(const Text& other);

			~Text();


			inline TextRenderable* accessRenderable() { return _renderable; }
		};
	}
}