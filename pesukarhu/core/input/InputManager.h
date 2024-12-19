#pragma once

#include "InputEvent.h"
#include "pesukarhu/Common.h"
#include "pesukarhu/core/Window.h"
#include <unordered_map>
#include <vector>
#include <utility>

namespace pk
{

    class InputManager
    {
    protected:
        std::vector<std::pair<KeyEvent*, void(KeyEvent::*)(InputKeyName, int, InputAction, int)>>						_keyEvents;
        std::vector<std::pair<MouseButtonEvent*, void(MouseButtonEvent::*)(InputMouseButtonName, InputAction, int)>>	_mouseButtonEvents;
        std::vector<std::pair<CursorPosEvent*, void(CursorPosEvent::*)(int, int)>>										_cursorPosEvents;
        std::vector<std::pair<ScrollEvent*, void(ScrollEvent::*)(double, double)>>										_scrollEvents;
        std::vector<std::pair<CharInputEvent*, void(CharInputEvent::*)(unsigned int codepoint)>>						_charInputEvents;
        std::vector<std::pair<WindowResizeEvent*, void(WindowResizeEvent::*)(int, int)>>								_windowResizeEvent;

        int _mouseX = 0;
        int _mouseY = 0;

    public:
        // Mainly for testing purposes!
        // To check immediate key and mouse down instead of having to always have some inputEvent
        std::unordered_map<InputKeyName, bool> keyDown;
        std::unordered_map<InputMouseButtonName, bool> mouseDown;

    public:
        InputManager();
        virtual ~InputManager();
        InputManager(const InputManager&) = delete;

        void addKeyEvent(KeyEvent* ev);
        void addMouseButtonEvent(MouseButtonEvent* ev);
        void addCursorPosEvent(CursorPosEvent* ev);
        void addScrollEvent(ScrollEvent* ev);
        void addCharInputEvent(CharInputEvent* ev);
        void addWindowResizeEvent(WindowResizeEvent* ev);

        void destroyEvents();

        void processKeyEvents(InputKeyName key, int scancode, InputAction action, int mods);
        void processMouseButtonEvents(InputMouseButtonName button, InputAction action, int mods);
        void processCursorPosEvents(int x, int y);
        void processScrollEvents(double dx, double dy);
        void processCharInputEvents(unsigned int codepoint);
        void processWindowResizeEvents(int w, int h);

        bool isKeyDown(InputKeyName key) const;
        bool isMouseButtonDown(InputMouseButtonName button) const;

        static InputManager* create(PlatformName platform, Window* pWindow);

        inline void setMousePos(int x, int y) { _mouseX = x; _mouseY = y; }
        inline int getMouseX() const { return _mouseX; }
        inline int getMouseY() const { return _mouseY; }
    };
}
