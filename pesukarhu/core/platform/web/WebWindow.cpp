
#include "WebWindow.h"
#include "pesukarhu/core/input/platform/web/WebInputManager.h"

#ifdef PK_BUILD_WEB
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace pk
{
    namespace web
    {
    #ifdef PK_BUILD_WEB
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
    #endif


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
        #ifdef PK_BUILD_WEB
            //_width = w;
            //_height = h;
            //resize_canvas(_width, _height);
            WebInputManager::query_window_surface_size(&_width, &_height);
            fit_page();

            if (_pSwapchain)
                _pSwapchain->triggerResize();
        #endif
        }
    }
}
