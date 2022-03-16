
#include "WebWindow.h"

#include <emscripten.h>
#include <emscripten/html5.h>

namespace pk
{
	namespace web
	{
		WebWindow::WebWindow(int width, int height) : 
			Window(width, height)
		{
			emscripten_set_canvas_element_size("#canvas", _width, _height);

		}

		WebWindow::~WebWindow()
		{

		}

	}
}