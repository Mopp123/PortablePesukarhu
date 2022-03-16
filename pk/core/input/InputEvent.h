#pragma once


namespace pk
{
	class KeyEvent
	{
	public:
		virtual ~KeyEvent() {};
		virtual void func(int key) = 0;
	};

	class WindowResizeEvent
	{
	public:
		virtual ~WindowResizeEvent() {};
		virtual void func(int key) = 0;
	};
}