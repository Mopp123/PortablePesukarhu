
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
			SceneManager& sceneManager = app->_sceneManager;
			sceneManager.handleSceneUpdate();

			// Begin renderpass...
			app->_pMasterRenderer->beginRenderPass();
			// Render all shit...
			for(const std::pair<ComponentType, Renderer*>& r : app->_renderers)
			{
				r.second->render();
			}
			// End renderpass...
			app->_pMasterRenderer->endRenderPass();
			
			// Attempt to detect and handle possible scene switching..
			sceneManager.handleSceneSwitching();
			app->_timing.update();
		}
	}

	Application* Application::s_pApplication = nullptr;

	Application::Application(
		std::string name,
		Window* window,
		Context* graphicsContext,
		InputManager* inputManager,
		Renderer* masterRenderer,
		std::unordered_map<ComponentType, Renderer*> renderers
	) :
		_name(name), 
		_pWindow(window),
		_pGraphicsContext(graphicsContext),
		_pInputManager(inputManager),
		_pMasterRenderer(masterRenderer),
		_renderers(renderers)
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

	void Application::switchScene(Scene* newScene)
	{
		_sceneManager.assignNextScene(newScene);
	}



	Application* Application::get()
	{
		return s_pApplication;
	}

}