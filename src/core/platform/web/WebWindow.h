#pragma once

#include "../../Window.h"

namespace pk
{
	namespace web
	{
		class WebWindow : public Window
		{
		public:

			WebWindow(int width, int height);
			~WebWindow();

		};
	}
}