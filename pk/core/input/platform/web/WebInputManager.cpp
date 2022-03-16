
#include "WebInputManager.h"

#include "../../../Application.h"

#include <emscripten.h>
#include <emscripten/html5.h>

#include "../../../Debug.h"

namespace pk
{
	namespace web
	{

		EM_JS(int, canvas_get_width, (), {
			return window.innerWidth;
		});

		EM_JS(int, canvas_get_height, (), {
			return window.innerHeight;
		});

		// Then call
		int width = canvas_get_width();
		int height = canvas_get_height();

		EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
		{
			Debug::log("INPUT EVENTING!!!\n");

			return true;
		}

		EM_BOOL ui_callback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData)
		{
			if (eventType == EMSCRIPTEN_EVENT_RESIZE)
			{
				int width = canvas_get_width();
				int height = canvas_get_height();

				Debug::log("w: " + std::to_string(width) + " h: " + std::to_string(height));
				
				(Application::get())->resizeWindow(width, height);
			}

			return true;
		}

		WebInputManager::WebInputManager()
		{
			emscripten_set_keydown_callback("#canvas", nullptr, true, key_callback);

			emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, ui_callback);
		}

		WebInputManager::~WebInputManager()
		{}
	}
}