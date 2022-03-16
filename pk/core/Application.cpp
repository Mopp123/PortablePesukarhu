
#include "Application.h"
#include "Debug.h"
#include "../Common.h"

#include <emscripten.h>


namespace pk
{

	void update()
	{
		Application* app = Application::s_application;

		if (app != nullptr)
		{
			app->_pMasterRenderer->render();
			
			app->_timing.update();
		}
	}

	Application* Application::s_application = nullptr;

	Application::Application(std::string name, MasterRenderer* mRenderer) :
		_name(name), _pMasterRenderer(mRenderer)
	{
		s_application = this;
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
}