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

	public:

		vec2 pos;
		
		TextRenderable(const std::string& txt) :
			Component(ComponentType::PK_RENDERABLE_TEXT),
			_txt(txt)
		{}

		inline const std::string& getStr() const { return _txt; }
	};
}