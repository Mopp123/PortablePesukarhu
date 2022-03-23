
#include "SceneManager.h"
#include "Debug.h"
#include "Application.h"

namespace pk
{
	void SceneManager::handleSceneUpdate()
	{
		if (_pCurrentScene)
			_pCurrentScene->update();


		// Submit all "renderable components" for rendering...
		
		Application* pApp = Application::get();
		Renderer* pGuiRenderer = pApp->getRenderer(ComponentType::PK_RENDERABLE_GUI);
		Renderer* pFontRenderer = pApp->getRenderer(ComponentType::PK_RENDERABLE_TEXT);

		for (const Component * const c_renderableGUI : _pCurrentScene->components[ComponentType::PK_RENDERABLE_GUI])
		{
			if(c_renderableGUI->isActive())
				pGuiRenderer->submit(c_renderableGUI);
		}

		for (const Component* const c_renderableText : _pCurrentScene->components[ComponentType::PK_RENDERABLE_TEXT])
		{
			if (c_renderableText->isActive())
				pFontRenderer->submit(c_renderableText);
		}
	}

	// triggers scene switching at the end of the frame
	void SceneManager::assignNextScene(Scene* newScene)
	{
		_pNextScene = newScene;
	}

	// Detects and handles scene switching
	void SceneManager::handleSceneSwitching()
	{
		if (_pNextScene != nullptr)
		{
			Debug::log("Attempting to switch scene");

			delete _pCurrentScene;

			_pCurrentScene = _pNextScene;
			_pCurrentScene->init();
			_pNextScene = nullptr;
		}
	}
}