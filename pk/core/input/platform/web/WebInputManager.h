#pragma once

#include "../../InputManager.h"

namespace pk
{
	namespace web
	{

		class WebInputManager : public InputManager
		{
		public:

			WebInputManager();
			~WebInputManager();


			static void query_window_size(int* outWidth, int* outHeight);
		};
	}
}