#pragma once


namespace pk
{

	class Window 
	{
	protected:

		int _width = 800;
		int _height = 600;

	public:

		Window(int width, int height);
		virtual ~Window();


		virtual void resize(int w, int h) {};
	};
}