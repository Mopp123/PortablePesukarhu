
#include "SceneManager.h"
#include "Debug.h"
#include "Application.h"
#include "../ecs/components/Transform.h"

namespace pk
{
	void SceneManager::handleSceneUpdate()
	{

		_pCurrentScene->update();

		// Update all "updateable" systems of the scene
		for (System* system : _pCurrentScene->systems[SystemType::PK_SYSTEM_TYPE_UPDATEABLE])
			((Updateable*)system)->update();

		// Submit all "renderable components" for rendering...
		
		Application* pApp = Application::get();
		// find gui renderers
		Renderer* pGuiRenderer =	pApp->getRenderer(ComponentType::PK_RENDERABLE_GUI);
		Renderer* pFontRenderer =	pApp->getRenderer(ComponentType::PK_RENDERABLE_TEXT);
		// find 3d renderers
		Renderer* pTerrainRenderer = pApp->getRenderer(ComponentType::PK_RENDERABLE_TERRAINTILE);

		// submitting renderables

		// TERRAIN TILES
		//*atm we want to pass just empty tMatrix, since tile figures out its' vertex positions from the tile component itself
		mat4 empty;
		for (const Component* const c_renderableTile : _pCurrentScene->components[ComponentType::PK_RENDERABLE_TERRAINTILE])
		{
			if (c_renderableTile->isActive())
			{
				pTerrainRenderer->submit(c_renderableTile, empty);
			}
		}

		// GUI
		for (const Component * const c_renderableGUI : _pCurrentScene->components[ComponentType::PK_RENDERABLE_GUI])
		{
			Component* rawTransform = _pCurrentScene->getComponent(c_renderableGUI->getEntity(), ComponentType::PK_TRANSFORM);
			if (rawTransform)
			{
				Transform* transform = (Transform*)rawTransform;
				pGuiRenderer->submit(c_renderableGUI, transform->getTransformationMatrix());
			}
		}
		// TEXT
		for (const Component* const c_renderableText : _pCurrentScene->components[ComponentType::PK_RENDERABLE_TEXT])
		{
			if (c_renderableText->isActive())
			{
				Component* rawTransform = _pCurrentScene->getComponent(c_renderableText->getEntity(), ComponentType::PK_TRANSFORM);
				if(rawTransform)
				{
					Transform* transform = (Transform*)rawTransform;
					pFontRenderer->submit(c_renderableText, transform->getTransformationMatrix());
				}
			}
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