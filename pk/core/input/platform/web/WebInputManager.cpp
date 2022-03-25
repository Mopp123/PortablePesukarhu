
#include "WebInputManager.h"

#include "../../../Application.h"
#include "../../../Debug.h"

#include <string>
#include <cstdint>

namespace pk
{
	namespace web
	{

		EM_JS(int, webwindow_get_width, (), {
			return window.innerWidth;
		});

		EM_JS(int, webwindow_get_height, (), {
			return window.innerHeight;
		});

		
		EM_BOOL keydown_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
		{
			WebInputManager* inputManager = (WebInputManager*)userData;

			InputKeyName key = inputManager->convert_to_keyname(keyEvent->key);
			int scancode = 0;
			int action = 0;
			
			inputManager->processKeyEvents(key, scancode, action, 0);
			
			unsigned char b1 = keyEvent->key[0];
			unsigned char b2 = keyEvent->key[1];

			
			unsigned int codepoint = inputManager->parseSpecialCharCodepoint(b2 == 0 ? (unsigned int)b1 : (unsigned int)b2);

			inputManager->processCharInputEvents(codepoint);

			return true;
		}
		EM_BOOL keyup_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
		{
			return true;
		}
		// like input char callback?
		EM_BOOL keypress_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
		{
			
			Debug::log(keyEvent->key);
			
			return true;
		}

		EM_BOOL mouse_down_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
		{
			WebInputManager* inputManager = (WebInputManager*)userData;

			InputMouseButtonName button = inputManager->convert_to_buttonname(mouseEvent->button);
			inputManager->processMouseButtonEvents(button, PK_INPUT_PRESS, 0);

			return true;
		}
		EM_BOOL mouse_up_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
		{
			WebInputManager* inputManager = (WebInputManager*)userData;

			InputMouseButtonName button = inputManager->convert_to_buttonname(mouseEvent->button);
			inputManager->processMouseButtonEvents(button, PK_INPUT_RELEASE, 0);

			return true;
		}

		EM_BOOL cursor_pos_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
		{
			WebInputManager* inputManager = (WebInputManager*)userData;

			int mx = mouseEvent->targetX;
			int my = mouseEvent->targetY;
			inputManager->setMousePos(mx, my);
			inputManager->processCursorPosEvents(mx, my);

			return true;
		}

		EM_BOOL ui_callback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData)
		{
			if (eventType == EMSCRIPTEN_EVENT_RESIZE)
			{
				int width = webwindow_get_width();
				int height = webwindow_get_height();

				Debug::log("w: " + std::to_string(width) + " h: " + std::to_string(height));
				
				(Application::get())->resizeWindow(width, height);
			}

			return true;
		}

		WebInputManager::WebInputManager()
		{
			//emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, key_callback);
			emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,		this, true, keydown_callback);
			emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,		this, true, keyup_callback);
			//emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,	this, true, keypress_callback);

			emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,	this, true, mouse_down_callback);
			emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,		this, true, mouse_up_callback);
			
			emscripten_set_mousemove_callback("canvas",							this, true, cursor_pos_callback);

			emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, ui_callback);
		}

		WebInputManager::~WebInputManager()
		{}


		void WebInputManager::query_window_size(int* outWidth, int* outHeight)
		{
			*outWidth = webwindow_get_width();
			*outHeight = webwindow_get_height();
		}


		unsigned int WebInputManager::parseSpecialCharCodepoint(unsigned int val) const
		{
			auto iter = _mapping_specialKeys.find(val);
			if (iter != _mapping_specialKeys.end())
				return iter->second;
			else
				return val;
		}
	}
}