﻿
#include "InputManager.h"

namespace pk
{

	
	InputManager::InputManager()
	{
	}

	InputManager::~InputManager()
	{
		destroyEvents();
	}


	void InputManager::addKeyEvent(KeyEvent* keyEvent)
	{
		_keyEvents.push_back(std::make_pair(keyEvent, &KeyEvent::func));
	}
	void InputManager::addMouseButtonEvent(MouseButtonEvent* ev)
	{
		_mouseButtonEvents.push_back(std::make_pair(ev, &MouseButtonEvent::func));
	}
	void InputManager::addCursorPosEvent(CursorPosEvent* ev)
	{
		_cursorPosEvents.push_back(std::make_pair(ev, &CursorPosEvent::func));
	}
	void InputManager::addScrollEvent(ScrollEvent* ev)
	{
		_scrollEvents.push_back(std::make_pair(ev, &ScrollEvent::func));
	}
	void InputManager::addCharInputEvent(CharInputEvent* ev)
	{
		_charInputEvents.push_back(std::make_pair(ev, &CharInputEvent::func));
	}



	void InputManager::destroyEvents()
	{
		for (std::pair<KeyEvent*, void(KeyEvent::*)(InputKeyName, int, int, int)>& ev : _keyEvents)
			delete ev.first;

		for (std::pair<MouseButtonEvent*, void(MouseButtonEvent::*)(InputMouseButtonName, InputAction, int)>& ev : _mouseButtonEvents)
			delete ev.first;

		for (std::pair<CursorPosEvent*, void(CursorPosEvent::*)(int, int)>& ev : _cursorPosEvents)
			delete ev.first;

		for (std::pair<ScrollEvent*, void(ScrollEvent::*)(double, double)>& ev : _scrollEvents)
			delete ev.first;

		for (std::pair<CharInputEvent*, void(CharInputEvent::*)(unsigned int)>& ev : _charInputEvents)
			delete ev.first;

		_keyEvents.clear();
		_mouseButtonEvents.clear();
		_cursorPosEvents.clear();
		_scrollEvents.clear();
		_charInputEvents.clear();
	}


	void InputManager::processKeyEvents(InputKeyName key, int scancode, int action, int mods)
	{
		for (std::pair<KeyEvent*, void(KeyEvent::*)(InputKeyName, int, int, int)>& ev : _keyEvents)
		{
			KeyEvent* caller = ev.first;
			void(KeyEvent:: * eventFunc)(InputKeyName, int, int, int) = ev.second;
			(caller->*eventFunc)(key, scancode, action, mods);
		}
	}

	void InputManager::processMouseButtonEvents(InputMouseButtonName button, InputAction action, int mods)
	{
		for (std::pair<MouseButtonEvent*, void(MouseButtonEvent::*)(InputMouseButtonName, InputAction, int)>& ev : _mouseButtonEvents)
		{
			MouseButtonEvent* caller = ev.first;
			void(MouseButtonEvent:: * eventFunc)(InputMouseButtonName, InputAction, int) = ev.second;
			(caller->*eventFunc)(button, action, mods);
		}
	}

	void InputManager::processCursorPosEvents(int mx, int my)
	{
		for (std::pair<CursorPosEvent*, void(CursorPosEvent::*)(int, int)>& ev : _cursorPosEvents)
		{
			CursorPosEvent* caller = ev.first;
			void(CursorPosEvent:: * eventFunc)(int, int) = ev.second;
			(caller->*eventFunc)(mx, my);
		}
	}
	void InputManager::processCharInputEvents(unsigned int codepoint)
	{
		for (std::pair<CharInputEvent*, void(CharInputEvent::*)(unsigned int)>& ev : _charInputEvents)
		{
			CharInputEvent* caller = ev.first;
			void(CharInputEvent:: * eventFunc)(unsigned int) = ev.second;
			(caller->*eventFunc)(codepoint);
		}
	}
}