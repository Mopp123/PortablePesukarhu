#pragma once

#include "../Component.h"
#include "../../../utils/pkmath.h"
#include <string>

namespace pk
{

	class TextRenderable : public Component
	{
	private:
		std::string _txt;
		bool _bold;

	public:

		vec3 color;

		TextRenderable(const std::string& txt, bool bold = false) :
			Component(ComponentType::PK_RENDERABLE_TEXT),
			_txt(txt),
			color(1, 1, 1),
			_bold(bold)
		{
		}


		inline std::string& accessStr() { return _txt; }
		inline const std::string& getStr() const { return _txt; }
		inline bool isBold() const { return _bold; }
	};
}