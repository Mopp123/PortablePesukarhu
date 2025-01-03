﻿#pragma once

#include <string>

#include "Window.h"
#include "input/InputManager.h"
#include "SceneManager.h"

#include "pesukarhu/Common.h"
#include "pesukarhu/graphics/Context.h"
#include "pesukarhu/graphics/MasterRenderer.h"
#include "pesukarhu/resources/ResourceManager.h"
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

        static PlatformName s_platform;
        static Application* s_pApplication;

        Window* _pWindow = nullptr;
        Context* _pGraphicsContext = nullptr;
        InputManager* _pInputManager = nullptr;

        ResourceManager _resourceManager;

        MasterRenderer* _pMasterRenderer;

    public:
        Application(
            PlatformName platform,
            std::string name,
            Window* window,
            Context* graphicsContext,
            InputManager* inputManager
        );
        ~Application();

        void run();

        void resizeWindow(int w, int h);
        void switchScene(Scene* newScene);

        static PlatformName get_platform();
        static Application* get();

        inline const Context * const getGraphicsContext() const { return _pGraphicsContext; }

        inline InputManager* accessInputManager() { return _pInputManager; }
        inline Scene* accessCurrentScene() { return _sceneManager.accessCurrentScene(); }

        inline const Window* const getWindow() const { return _pWindow; }
        inline const Scene* const getCurrentScene() const { return _sceneManager.getCurrentScene(); }

        inline MasterRenderer& getMasterRenderer() { return *_pMasterRenderer; }
        inline ResourceManager& getResourceManager() { return _resourceManager; }
        inline const ResourceManager& getResourceManager() const { return _resourceManager; }

        inline bool isRunning() const { return _running; }

    private:
        friend void update();
    };

}
