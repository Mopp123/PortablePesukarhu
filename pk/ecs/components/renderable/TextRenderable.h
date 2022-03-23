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

		vec3 color;
		
		TextRenderable(const std::string& txt) :
			Component(ComponentType::PK_RENDERABLE_TEXT),
			_txt(txt),
			color(1,1,1)
		{
		}

		inline const std::string& getStr() const { return _txt; }
	};
}