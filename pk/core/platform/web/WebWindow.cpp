
#include "WebWindow.h"

#include <emscripten.h>
#include <emscripten/html5.h>

namespace pk
{
	namespace web
	{

		EM_JS(void, resize_canvas, (int w, int h), {
			var c = document.getElementById('canvas');
			c.width = w;
			c.height = h;
		});

		WebWindow::WebWindow(int width, int height) : 
			Window(width, height)
		{
			emscripten_set_canvas_element_size("#canvas", _width, _height);

		}

		WebWindow::~WebWindow()
		{

		}


		void WebWindow::resize(int w, int h)
		{
			_width = w;
			_height = h;

			resize_canvas(_width, _height);
		}
	}
}