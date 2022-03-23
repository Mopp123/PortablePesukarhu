
#include "Application.h"
#include "Debug.h"
#include "../Common.h"

#include "../utils/pkmath.h"

#include <emscripten.h>


namespace pk
{

	void update()
	{
		Application* app = Application::s_pApplication;

		mat4 projMat = create_proj_mat_ortho(0, 800, 600, 0, 0.0f, 100.0f);
		mat4 viewMat;

		if (app != nullptr)
		{
			SceneManager& sceneManager = app->_sceneManager;
			sceneManager.handleSceneUpdate();

			// All "top level" rendering stuff...
			Renderer* masterRenderer = app->_pMasterRenderer;
			masterRenderer->beginFrame(); // (in Vulkan, acquire swapchain img here.., and maybe begin the primary cmd buf)
			masterRenderer->beginRenderPass();
			// 'Render all "actual renderers"' (in Vulkan (re)record all secondary cmdbufs here)
			for(const std::pair<ComponentType, Renderer*>& r : app->_renderers)
			{
				r.second->render(projMat, viewMat);
			}
			masterRenderer->endRenderPass();
			masterRenderer->endFrame(); // (submit cmdbuf[currentFrameIndex] to swapchain for execution, AND attempt to present the "top" swapchain image)

			// Detect and handle possible scene switching..
			sceneManager.handleSceneSwitching();
			app->_timing.update();

			Debug::log("delta: " + std::to_string(Timing::get_delta_time()));
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
		emscripten_set_main_loop(update, 60, 1);
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