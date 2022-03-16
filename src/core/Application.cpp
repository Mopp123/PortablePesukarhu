
#include "Application.h"
#include "Debug.h"
#include "../Common.h"

#include <emscripten.h>


namespace pk
{

	void update()
	{
		if (Application::s_application != nullptr)
		{
			Application::s_application->_pMasterRenderer->render();
		}
		else
		{
			Debug::log("hmm... no mitahan vittua sitten...?", Debug::MessageType::PK_FATAL_ERROR);
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