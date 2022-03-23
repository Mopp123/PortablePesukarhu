#pragma once

#include "Scene.h"

namespace pk
{
	class SceneManager
	{
	private:

		Scene* _pCurrentScene = nullptr;
		Scene* _pNextScene = nullptr;

	public:

		void handleSceneUpdate();

		// triggers scene switching at the end of the frame
		void assignNextScene(Scene* newScene);

		// Detects and handles scene switching
		void handleSceneSwitching();

		inline Scene* getCurrentScene() { return _pCurrentScene; }
	};
}