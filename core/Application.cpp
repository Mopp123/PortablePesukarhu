#include "Application.h"
#include "../Common.h"
#include "../utils/pkmath.h"

#include "Debug.h"
#include <iostream>
#include <emscripten.h>


namespace pk
{
    void update()
    {
        Application* app = Application::s_pApplication;

        if(app)
        {
            const Scene* currentScene = app->getCurrentScene();
            if (currentScene)
            {
                const Camera* const activeCam = currentScene->activeCamera;

                SceneManager& sceneManager = app->_sceneManager;
                sceneManager.handleSceneUpdate();

                if (activeCam != nullptr)
                {
                    MasterRenderer* masterRenderer = app->_pMasterRenderer;
                    //masterRenderer->render(*activeCam);
                }
                else
                {
                    Debug::log("Scene doesn't have active camera", Debug::MessageType::PK_ERROR);
                }
                //Debug::log("delta: " + std::to_string(Timing::get_delta_time()));
            }
            // Detect and handle possible scene switching..

            app->_sceneManager.handleSceneSwitching();
            app->_timing.update();

            //GLenum err = glGetError();
            //if (err != GL_NO_ERROR)
            //	std::cout << "GL ERROR!: " << err << std::endl;
        }
    }

    Application* Application::s_pApplication = nullptr;

    Application::Application(
        std::string name,
        Window* window,
        Context* graphicsContext,
        InputManager* inputManager,
        MasterRenderer* pMasterRenderer
    ) :
        _name(name),
        _pWindow(window),
        _pGraphicsContext(graphicsContext),
        _pInputManager(inputManager),
        _pMasterRenderer(pMasterRenderer)
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
        _pMasterRenderer->handleWindowResize(w, h);
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
