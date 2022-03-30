#pragma once

#include "InputEvent.h"
#include <vector>
#include <utility>

namespace pk
{
	
	class InputManager
	{
	protected:

		std::vector<std::pair<KeyEvent*, void(KeyEvent::*)(InputKeyName, int, InputAction, int)>>								_keyEvents;
		std::vector<std::pair<MouseButtonEvent*, void(MouseButtonEvent::*)(InputMouseButtonName, InputAction, int)>>	_mouseButtonEvents;
		std::vector<std::pair<CursorPosEvent*, void(CursorPosEvent::*)(int, int)>>										_cursorPosEvents;
		std::vector<std::pair<ScrollEvent*, void(ScrollEvent::*)(double, double)>>				_scrollEvents;
		std::vector<std::pair<CharInputEvent*, void(CharInputEvent::*)(unsigned int codepoint)>>			_charInputEvents;
		
		int _mouseX = 0;
		int _mouseY = 0;


	public:

		InputManager();
		virtual ~InputManager();

		void addKeyEvent(KeyEvent* ev);
		void addMouseButtonEvent(MouseButtonEvent* ev);
		void addCursorPosEvent(CursorPosEvent* ev);
		void addScrollEvent(ScrollEvent* ev);
		void addCharInputEvent(CharInputEvent* ev);

		void destroyEvents();

		void processKeyEvents(InputKeyName key, int scancode, InputAction action, int mods);
		void processMouseButtonEvents(InputMouseButtonName button, InputAction action, int mods);
		void processCursorPosEvents(int x, int y);
		void processCharInputEvents(unsigned int codepoint);

		inline void setMousePos(int x, int y) { _mouseX = x; _mouseY = y; }
		inline int getMouseX() const { return _mouseX; }
		inline int getMouseY() const { return _mouseY; }
	};

}