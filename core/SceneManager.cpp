#include "SceneManager.h"
#include "Debug.h"
#include "Application.h"
#include "ecs/components/renderable/GUIRenderable.h"
#include "../ecs/components/Transform.h"

#include <chrono>


namespace pk
{
    static void submit_renderable(
        Scene* pScene,
        ComponentPool& renderablePool,
        ComponentPool& transformPool,
        const std::vector<Entity>& entities,
        Renderer* pRenderer,
        uint32_t requiredComponentMask,
        ComponentType requiredAdditionalComponent
    )
    {
        void* pCustomData = nullptr;
        size_t customDataSize = 0;
        for (const Entity& e : entities)
        {
            if ((e.componentMask & requiredComponentMask) == requiredComponentMask)
            {
                Component* pRenderable = (Component*)renderablePool[e.id];
                Transform* pTransform = (Transform*)transformPool[e.id];
                if (!pRenderable->isActive())
                    continue;

                if (requiredAdditionalComponent != ComponentType::PK_EMPTY)
                {
                    Component* pAdditionalComponent = pScene->getComponent(
                        e.id,
                        requiredAdditionalComponent
                    );
                    pCustomData = (void*)pAdditionalComponent;
                    customDataSize = sizeof(Component*); // ..ptr size should be same for any ptr
                }

                pRenderer->submit(
                    pRenderable,
                    pTransform->getTransformationMatrix(),
                    pCustomData,
                    customDataSize
                );
            }
        }
    }


    void SceneManager::handleSceneUpdate()
    {
        Application* pApp = Application::get();

        #ifdef PK_DEBUG_FULL
        bool recordPerf = false;
        std::chrono::time_point<std::chrono::high_resolution_clock> startUpdatePerfRecord;
        std::chrono::time_point<std::chrono::high_resolution_clock> startSystemPerfRecord;
        std::chrono::time_point<std::chrono::high_resolution_clock> startLateUpdatePerfRecord;
        std::chrono::time_point<std::chrono::high_resolution_clock> startRenderSubmitPerfRecord;
        std::chrono::duration<float> updateDelta;
        std::chrono::duration<float> systemDelta;
        std::chrono::duration<float> lateUpdateDelta;
        std::chrono::duration<float> renderSubmitDelta;

        InputManager* pInputManager = pApp->accessInputManager();
        if (pInputManager->isKeyDown(PK_INPUT_KEY_F1))
        {
            recordPerf = true;
            startUpdatePerfRecord = std::chrono::high_resolution_clock::now();
        }
        #endif

        _pCurrentScene->update();

        // Record perf in debug mode using F-keys
        #ifdef PK_DEBUG_FULL
        if (recordPerf)
        {
            updateDelta = std::chrono::high_resolution_clock::now() - startUpdatePerfRecord;
            startSystemPerfRecord = std::chrono::high_resolution_clock::now();
        }
        #endif

        // Update all systems of the scene
        for (System* system : _pCurrentScene->systems)
            system->update(_pCurrentScene);

        #ifdef PK_DEBUG_FULL
        if (recordPerf)
        {
            systemDelta = std::chrono::high_resolution_clock::now() - startSystemPerfRecord;
            startLateUpdatePerfRecord = std::chrono::high_resolution_clock::now();
        }
        #endif

        _pCurrentScene->lateUpdate();

        #ifdef PK_DEBUG_FULL
        if (recordPerf)
        {
            lateUpdateDelta = std::chrono::high_resolution_clock::now() - startLateUpdatePerfRecord;
            startRenderSubmitPerfRecord = std::chrono::high_resolution_clock::now();
        }
        #endif

        // Submit all "renderable components" for rendering...
        // NOTE: This has to be done here since need quarantee that all transforms and shit has been
        // properly updated before submission!
        ComponentPool& transformPool = _pCurrentScene->componentPools[ComponentType::PK_TRANSFORM];
        MasterRenderer& masterRenderer = pApp->getMasterRenderer();
        std::map<ComponentType, Renderer*>& renderers = masterRenderer.accessRenderers();
        std::map<ComponentType, Renderer*>::iterator rIt;
        const std::vector<Entity>& entities = _pCurrentScene->entities;
        for (rIt = renderers.begin(); rIt != renderers.end(); ++rIt)
        {
            const ComponentType& renderableType = rIt->first;
            // TODO: make this more clever..
            ComponentType requiredAdditionalComponent = renderableType == ComponentType::PK_RENDERABLE_SKINNED ? ComponentType::PK_ANIMATION_DATA : ComponentType::PK_EMPTY;
            submit_renderable(
                _pCurrentScene,
                _pCurrentScene->componentPools[renderableType],
                transformPool,
                entities,
                rIt->second,
                ComponentType::PK_TRANSFORM | renderableType,
                requiredAdditionalComponent
            );
        }

        #ifdef PK_DEBUG_FULL
        if (recordPerf)
        {
            renderSubmitDelta = std::chrono::high_resolution_clock::now() - startLateUpdatePerfRecord;
            Debug::log(
                "@SceneManager::handleSceneUpdate "
                "Performance record results:\n"
                "   Scene update: " + std::to_string(updateDelta.count()) + "\n"
                "   Scene late update: " + std::to_string(lateUpdateDelta.count()) + "\n"
                "   Systems update: " + std::to_string(systemDelta.count()) + "\n"
                "   Renderer submitting: " + std::to_string(renderSubmitDelta.count()) + "\n"
            );
        }
        #endif
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
            Debug::log("Attempting to switch scene...");
            Application* pApp = Application::get();
            pApp->getMasterRenderer().handleSceneSwitch();
            pApp->accessInputManager()->destroyEvents();
            pApp->getResourceManager().freeResources();
            delete _pCurrentScene;
            _pCurrentScene = _pNextScene;
            _pCurrentScene->init();

            // TODO: Deal with this shit at some point!
            // IMPORTANT: Also camera shouldn't have "2D proj mat"!!!
            //  -> Some kind of canvas shit for all 2d stuff to separate 2d from 3d
            //  rendering things completely!!!
            if (_pCurrentScene->activeCamera == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@SceneManager::handleSceneSwitching "
                    "New scene didn't create or assign active camera entity/component. "
                    "Currently this is required!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                delete _pCurrentScene;
            }
            if (_pCurrentScene->directionalLight == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@SceneManager::handleSceneSwitching "
                    "New scene didn't create or assign active directional light entity/component. "
                    "Currently this is required!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                delete _pCurrentScene;
            }
            _pNextScene = nullptr;
        }
    }
}
