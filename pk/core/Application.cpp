
#include "Application.h"
#include "Debug.h"
#include "../Common.h"

#include <emscripten.h>


namespace pk
{

	void update()
	{
		Application* app = Application::s_pApplication;

		if (app != nullptr)
		{
			app->_pMasterRenderer->render();
			
			app->_timing.update();
		}
	}

	Application* Application::s_pApplication = nullptr;

	Application::Application(std::string name, Window* window, MasterRenderer* mRenderer) :
		_name(name), _pWindow(window), _pMasterRenderer(mRenderer)
	{
		s_pApplication = this;
	}
	Application::~Application()
	{
		Debug::log("App destroyed");
	}


	void Application::run()
	{
	#ifdef PK_BUILD_WEB
		emscripten_set_main_loop(update, 0, 1);
	#else
		while (_running)
		{
			update();
		}
	#endif
	}

	void Application::resizeWindow(int w, int h)
	{
		_pWindow->resize(w, h);
		_pMasterRenderer->resize(w, h);
	}


	Application* Application::get()
	{
		return s_pApplication;
	}

}