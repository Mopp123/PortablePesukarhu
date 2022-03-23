#pragma once

#include "../../../utils/pkmath.h"
#include "../../../core/Application.h"
#include "../../../core/Window.h"
#include "../../components/Transform.h"

namespace pk
{
	namespace ui
	{
		enum ConstraintType
		{
			PIXEL_LEFT = 1,
			PIXEL_RIGHT = 2,
			PIXEL_TOP = 3,
			PIXEL_BOTTOM = 4,
			PIXEL_CENTER_HORIZONTAl = 5,
			PIXEL_CENTER_VERTICAL = 6
		};

		class Constraint
		{
		public:
			ConstraintType type = PIXEL_LEFT;
			float value = 0.0f;

			void apply(Transform& transformToConstraint)
			{
				const Window* const win = Application::get()->getWindow();
				const float windowWidth = win->getWidth();
				const float windowHeight = win->getHeight();

				mat4& tMat = transformToConstraint.accessTransformationMatrix();
				
				const float& transformWidth = tMat[0 + 0 * 4];
				const float& transformHeight = tMat[1 + 1 * 4];

				switch (type)
				{
				case pk::ui::PIXEL_LEFT:		tMat[0 + 3 * 4] = value;									break;
				case pk::ui::PIXEL_RIGHT:		tMat[0 + 3 * 4] = windowWidth - value - transformWidth;		break;
				case pk::ui::PIXEL_BOTTOM:		tMat[1 + 3 * 4] = value;									break;
				case pk::ui::PIXEL_TOP:			tMat[1 + 3 * 4] = windowHeight - value - transformHeight;	break;
				case pk::ui::PIXEL_CENTER_HORIZONTAl:break;
				case pk::ui::PIXEL_CENTER_VERTICAL:break;

				default:
					break;
				}
			}
		};
	}
}