
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

	void InputManager::destroyEvents()
	{
		for (std::pair<KeyEvent*, void(KeyEvent::*)(int)>& ev : _keyEvents)
			delete ev.first;

		_keyEvents.clear();
	}



	InputManager* InputManager::get()
	{
		return s_inputManager;
	}
}