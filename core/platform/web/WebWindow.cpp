
#include "WebWindow.h"
#include "../../input/platform/web/WebInputManager.h"
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


		EM_JS(void, fit_page, (), {
			var c = document.getElementById('canvas');
  			c.width  = window.innerWidth;
  			c.height = window.innerHeight;
		});


		WebWindow::WebWindow() : 
			Window(0,0)
		{
			resize(0, 0);
		}

		WebWindow::WebWindow(int width, int height) : 
			Window(width, height)
		{
			//emscripten_set_canvas_element_size("#canvas", _width, _height);
			resize(0, 0);
		}

		WebWindow::~WebWindow()
		{

		}

		// Currently handles the resizing to fit canvas' dimensions to the page's
		void WebWindow::resize(int w, int h)
		{
			//_width = w;
			//_height = h;
			//resize_canvas(_width, _height);
			WebInputManager::query_window_size(&_width, &_height);
			fit_page();
		}
	}
}
