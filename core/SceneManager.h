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

        inline const Scene* const getCurrentScene() const { return _pCurrentScene; }
        inline Scene* accessCurrentScene() { return _pCurrentScene; }
    };
}
