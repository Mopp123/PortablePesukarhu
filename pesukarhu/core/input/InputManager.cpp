﻿
#include "InputManager.h"
#include "pesukarhu/core/Debug.h"

#ifdef PK_BUILD_WEB
    #include "platform/web/WebInputManager.h"
#elif defined(PK_BUILD_DESKTOP)
    #include "platform/desktop/DesktopInputManager.h"
#endif


namespace pk
{
    InputManager::InputManager()
    {
    }

    InputManager::~InputManager()
    {
        Debug::log("Destroying input events...");
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
    void InputManager::addWindowResizeEvent(WindowResizeEvent* ev)
    {
        _windowResizeEvent.push_back(std::make_pair(ev, &WindowResizeEvent::func));
    }


    void InputManager::destroyEvents()
    {
        Debug::log("___TEST___deleting input events from manager!");
        for (std::pair<KeyEvent*, void(KeyEvent::*)(InputKeyName, int, InputAction, int)>& ev : _keyEvents)
            delete ev.first;

        for (std::pair<MouseButtonEvent*, void(MouseButtonEvent::*)(InputMouseButtonName, InputAction, int)>& ev : _mouseButtonEvents)
            delete ev.first;

        for (std::pair<CursorPosEvent*, void(CursorPosEvent::*)(int, int)>& ev : _cursorPosEvents)
            delete ev.first;

        for (std::pair<ScrollEvent*, void(ScrollEvent::*)(double, double)>& ev : _scrollEvents)
            delete ev.first;

        for (std::pair<CharInputEvent*, void(CharInputEvent::*)(unsigned int)>& ev : _charInputEvents)
            delete ev.first;

        for (std::pair<WindowResizeEvent*, void(WindowResizeEvent::*)(int, int)>& ev : _windowResizeEvent)
            delete ev.first;

        _keyEvents.clear();
        _mouseButtonEvents.clear();
        _cursorPosEvents.clear();
        _scrollEvents.clear();
        _charInputEvents.clear();
        _windowResizeEvent.clear();
    }


    void InputManager::processKeyEvents(InputKeyName key, int scancode, InputAction action, int mods)
    {
        for (std::pair<KeyEvent*, void(KeyEvent::*)(InputKeyName, int, InputAction, int)>& ev : _keyEvents)
        {
            KeyEvent* caller = ev.first;
            void(KeyEvent:: * eventFunc)(InputKeyName, int, InputAction, int) = ev.second;
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

    void InputManager::processScrollEvents(double dx, double dy)
    {
        for (std::pair<ScrollEvent*, void(ScrollEvent::*)(double, double)>& ev : _scrollEvents)
        {
            ScrollEvent* caller = ev.first;
            void(ScrollEvent:: * eventFunc)(double, double) = ev.second;
            (caller->*eventFunc)(dx, dy);
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

    void InputManager::processWindowResizeEvents(int w, int h)
    {
        for (std::pair<WindowResizeEvent*, void(WindowResizeEvent::*)(int, int)>& ev : _windowResizeEvent)
        {
            WindowResizeEvent* caller = ev.first;
            void(WindowResizeEvent:: * eventFunc)(int, int) = ev.second;
            (caller->*eventFunc)(w, h);
        }
    }

    bool InputManager::isKeyDown(InputKeyName key) const
    {
        std::unordered_map<InputKeyName, bool>::const_iterator it = keyDown.find(key);
        if (it != keyDown.end())
            return it->second;
        return false;
    }

    bool InputManager::isMouseButtonDown(InputMouseButtonName button) const
    {
        std::unordered_map<InputMouseButtonName, bool>::const_iterator it = mouseDown.find(button);
        if (it != mouseDown.end())
            return it->second;
        return false;
    }

    InputManager* InputManager::create(PlatformName platform, Window* pWindow)
    {
        #ifdef PK_BUILD_WEB
            return new web::WebInputManager;
        #elif defined(PK_BUILD_DESKTOP)
            return new desktop::DesktopInputManager((desktop::DesktopWindow*)pWindow);
        #else
            Debug::log(
                "@InputManager::create "
                "Failed to create InputManager. Invalid build target! "
                "Available targets: web, desktop(linux)",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr
        #endif
    }
}
