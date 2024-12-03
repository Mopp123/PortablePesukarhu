#include "Application.h"
#include "Common.h"
#include "Pesukarhu/utils/pkmath.h"

#include "Debug.h"
#include <iostream>

#include <chrono>

#ifdef PK_PLATFORM_WEB
#include <emscripten.h>
#endif


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
                const Camera* pCamera = (const Camera*)currentScene->getComponent(currentScene->activeCamera, ComponentType::PK_CAMERA);
                const Transform* pCameraTransform = (const Transform*)currentScene->getComponent(currentScene->activeCamera, ComponentType::PK_TRANSFORM);

                SceneManager& sceneManager = app->_sceneManager;
                sceneManager.handleSceneUpdate();

                MasterRenderer& masterRenderer = app->getMasterRenderer();;
                #ifdef PK_DEBUG_FULL
                std::chrono::time_point<std::chrono::high_resolution_clock> startRenderPerfRecord;
                bool recordRenderPerf = false;
                if (app->accessInputManager()->isKeyDown(PK_INPUT_KEY_F1))
                {
                    recordRenderPerf = true;
                    startRenderPerfRecord = std::chrono::high_resolution_clock::now();
                }
                #endif
                if (pCamera && pCameraTransform)
                {
                    const mat4& camTMat = pCameraTransform->getTransformationMatrix();
                    vec3 camPos(camTMat[0 + 3 * 4], camTMat[1 + 3 * 4], camTMat[2 + 3 * 4]);
                    mat4 viewMatrix = camTMat;
                    viewMatrix.inverse();
                    masterRenderer.render(pCamera->getProjMat3D(), viewMatrix, camPos);
                }
                else
                {
                    Debug::log("Scene doesn't have active camera", Debug::MessageType::PK_ERROR);
                }
                masterRenderer.flush();
                #ifdef PK_DEBUG_FULL
                if (recordRenderPerf)
                {
                    std::chrono::duration<float> delta = std::chrono::high_resolution_clock::now() - startRenderPerfRecord;
                    Debug::log(
                        "@Application::update(Emscripten) "
                        "   Rendering took: " + std::to_string(delta.count())
                    );
                }
                #endif
                //Debug::log("delta: " + std::to_string(Timing::get_delta_time()));
            }
            // Detect and handle possible scene switching..
            app->_sceneManager.handleSceneSwitching();
            app->_timing.update();
        }
    }

    uint32_t Application::s_platform = PK_PLATFORM_ID_NONE;
    Application* Application::s_pApplication = nullptr;

    Application::Application(
        uint32_t platform,
        std::string name,
        Window* window,
        Context* graphicsContext,
        InputManager* inputManager
    ) :
        _name(name),
        _pWindow(window),
        _pGraphicsContext(graphicsContext),
        _pInputManager(inputManager)
    {
        s_platform = platform;
        s_pApplication = this;

        // This is done because swapchain creation requires window
        // and graphics context to exist.
        // NOTE: "Offscreen usage" isn't supported atm (cant create Application without window)
        if (_pWindow)
            _pWindow->createSwapchain();
        else
            Debug::log(
                "@Application::Application Window not assigned. "
                "Currently Application creation is not allowed without window",
                Debug::MessageType::PK_FATAL_ERROR
            );

        _resourceManager.createDefaultResources();

        // Need to create renderers after swapchain and default resource creation!
        _pMasterRenderer = new MasterRenderer;
        // Need to init renderers' pipelines after MasterRenderer creation since
        // some of those may need to access master renderer's default descriptorset layouts, etc..
        _pMasterRenderer->init();
    }

    Application::~Application()
    {
        Debug::log("App destroyed");
    }

    void Application::run()
    {
#ifdef PK_PLATFORM_WEB
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
    }

    void Application::switchScene(Scene* newScene)
    {
        _sceneManager.assignNextScene(newScene);
    }

    uint32_t Application::get_platform()
    {
        return s_platform;
    }

    Application* Application::get()
    {
        return s_pApplication;
    }
}
