
#include "WebInputManager.h"

#include <emscripten.h>
#include <emscripten/html5.h>

#include "../../../Debug.h"

namespace pk
{
	namespace web
	{

		EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
		{
			Debug::log("INPUT EVENTING!!!\n");

			return true;
		}

		WebInputManager::WebInputManager()
		{
			emscripten_set_keydown_callback("#canvas", nullptr, true, key_callback);
		}

		WebInputManager::~WebInputManager()
		{}
	}
}