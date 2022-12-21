#pragma once

#include <string>

#include "Window.h"
#include "input/InputManager.h"
#include "SceneManager.h"
#include "../graphics/Context.h"

#include "../graphics/Renderer.h"
#include "Timing.h"

#include <vector>
#include <map>
#include <unordered_map>

#include <memory>


namespace pk
{
    class Application
    {
    private:
        std::string _name;
        bool _running = true;

        Timing _timing;
        SceneManager _sceneManager;

        static Application* s_pApplication;

        Window* _pWindow = nullptr;
        InputManager* _pInputManager = nullptr;
        Context* _pGraphicsContext = nullptr;

        Renderer* _pMasterRenderer = nullptr;
        std::map<ComponentType, Renderer*> _renderers;

    public:
        Application(
                std::string name,
                Window* window,
                Context* graphicsContext,
                InputManager* inputManager,
                Renderer* masterRenderer,
                std::map<ComponentType, Renderer*> renderers
                );
        ~Application();

        void run();

        void resizeWindow(int w, int h);
        void switchScene(Scene* newScene);

        static Application* get();

        inline InputManager* accessInputManager() { return _pInputManager; }
        inline Scene* accessCurrentScene() { return _sceneManager.accessCurrentScene(); }

        inline const Window* const getWindow() const { return _pWindow; }
        inline const Scene* const getCurrentScene() const { return _sceneManager.getCurrentScene(); }

        inline Renderer* const getRenderer(ComponentType renderableType) { return _renderers[renderableType]; }

        inline bool isRunning() const { return _running; }

    private:
        friend void update();
    };

}
