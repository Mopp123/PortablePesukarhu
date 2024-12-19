#pragma once

#include "pesukarhu/core/Window.h"


namespace pk
{
	namespace web
	{
		class WebWindow : public Window
		{
		public:
			WebWindow(int width, int height);
			~WebWindow();

			virtual void resize(int w, int h);
		};
	}
}
