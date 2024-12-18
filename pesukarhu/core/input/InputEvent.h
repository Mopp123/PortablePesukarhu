#pragma once


namespace pk
{
	enum InputKeyName
	{
		PK_INPUT_KEY_0,
		PK_INPUT_KEY_1,
		PK_INPUT_KEY_2,
		PK_INPUT_KEY_3,
		PK_INPUT_KEY_4,
		PK_INPUT_KEY_5,
		PK_INPUT_KEY_6,
		PK_INPUT_KEY_7,
		PK_INPUT_KEY_8,
		PK_INPUT_KEY_9,


		PK_INPUT_KEY_F1,
		PK_INPUT_KEY_F2,
		PK_INPUT_KEY_F3,
		PK_INPUT_KEY_F4,
		PK_INPUT_KEY_F5,
		PK_INPUT_KEY_F6,
		PK_INPUT_KEY_F7,
		PK_INPUT_KEY_F8,
		PK_INPUT_KEY_F9,
		PK_INPUT_KEY_F10,
		PK_INPUT_KEY_F11,
		PK_INPUT_KEY_F12,

		PK_INPUT_KEY_Q,
		PK_INPUT_KEY_W,
		PK_INPUT_KEY_E,
		PK_INPUT_KEY_R,
		PK_INPUT_KEY_T,
		PK_INPUT_KEY_Y,
		PK_INPUT_KEY_U,
		PK_INPUT_KEY_I,
		PK_INPUT_KEY_O,
		PK_INPUT_KEY_P,
		PK_INPUT_KEY_A,
		PK_INPUT_KEY_S,
		PK_INPUT_KEY_D,
		PK_INPUT_KEY_F,
		PK_INPUT_KEY_G,
		PK_INPUT_KEY_H,
		PK_INPUT_KEY_J,
		PK_INPUT_KEY_K,
		PK_INPUT_KEY_L,
		PK_INPUT_KEY_Z,
		PK_INPUT_KEY_X,
		PK_INPUT_KEY_C,
		PK_INPUT_KEY_V,
		PK_INPUT_KEY_B,
		PK_INPUT_KEY_N,
		PK_INPUT_KEY_M,

		PK_INPUT_KEY_UP,
		PK_INPUT_KEY_DOWN,
		PK_INPUT_KEY_LEFT,
		PK_INPUT_KEY_RIGHT,

		PK_INPUT_KEY_SPACE,
		PK_INPUT_KEY_BACKSPACE,
		PK_INPUT_KEY_ENTER,
		PK_INPUT_KEY_LCTRL,
		PK_INPUT_KEY_SHIFT,
		PK_INPUT_KEY_TAB,
		PK_INPUT_KEY_ESCAPE
	};

	enum InputMouseButtonName
	{
		PK_INPUT_MOUSE_LEFT,
		PK_INPUT_MOUSE_MIDDLE,
		PK_INPUT_MOUSE_RIGHT
	};

	enum InputAction
	{
		PK_INPUT_PRESS,
		PK_INPUT_RELEASE
	};

	class KeyEvent
	{
	public:
		virtual ~KeyEvent() {};
		virtual void func(InputKeyName key, int scancode, InputAction action, int mods) = 0;
	};

	class MouseButtonEvent
	{
	public:
		virtual ~MouseButtonEvent() {};
		virtual void func(InputMouseButtonName button, InputAction action, int mods) = 0;
	};

	class CursorPosEvent
	{
	public:
		virtual ~CursorPosEvent() {};
		virtual void func(int x, int y) = 0;
	};

	class ScrollEvent
	{
	public:
		virtual ~ScrollEvent() {};
		virtual void func(double xOffset, double yOffset) = 0;
	};

	class CharInputEvent
	{
	public:
		virtual ~CharInputEvent() {};
		virtual void func(unsigned int codepoint) = 0;
	};

	class WindowResizeEvent
	{
	public:
		virtual ~WindowResizeEvent() {};
		virtual void func(int w, int h) = 0;
	};
}
