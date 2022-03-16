#pragma once

#include "InputEvent.h"
#include <vector>
#include <utility>

namespace pk
{

	class InputManager
	{
	protected:

		std::vector<std::pair<KeyEvent*, void(KeyEvent::*)(int)>> _keyEvents;
		
		static InputManager* s_inputManager;

	public:

		InputManager();
		virtual ~InputManager();

		void addKeyEvent(KeyEvent* ev);
		void destroyEvents();

		static InputManager* get();
	};

}