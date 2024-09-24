#include "SceneManager.h"
#include "Debug.h"
#include "Application.h"
#include "ecs/components/renderable/GUIRenderable.h"
#include "../ecs/components/Transform.h"


namespace pk
{
    static void submit_renderable(
        ComponentPool& renderablePool,
        ComponentPool& transformPool,
        const std::vector<Entity>& entities,
        Renderer* pRenderer,
        uint32_t requiredComponentMask
    )
    {
        for (const Entity& e : entities)
        {
            if ((e.componentMask & requiredComponentMask) == requiredComponentMask)
            {
                Component* pRenderable = (Component*)renderablePool[e.id];
                Transform* pTransform = (Transform*)transformPool[e.id];
                if (!pRenderable->isActive())
                    continue;

                pRenderer->submit(pRenderable, pTransform->getTransformationMatrix());
            }
        }
    }


    void SceneManager::handleSceneUpdate()
    {
        _pCurrentScene->update();

        // Update all systems of the scene
        for (System* system : _pCurrentScene->systems)
            system->update(_pCurrentScene);

        // Submit all "renderable components" for rendering...
        // NOTE: This has to be done here since need quarantee that all transforms and shit has been
        // properly updated before submission!
        Application* pApp = Application::get();
        ComponentPool& transformPool = _pCurrentScene->componentPools[ComponentType::PK_TRANSFORM];
        MasterRenderer& masterRenderer = pApp->getMasterRenderer();
        std::map<ComponentType, Renderer*>& renderers = masterRenderer.accessRenderers();
        std::map<ComponentType, Renderer*>::iterator rIt;
        const std::vector<Entity>& entities = _pCurrentScene->entities;
        for (rIt = renderers.begin(); rIt != renderers.end(); ++rIt)
        {
            const ComponentType& renderableType = rIt->first;
            submit_renderable(
                _pCurrentScene->componentPools[renderableType],
                transformPool,
                entities,
                rIt->second,
                ComponentType::PK_TRANSFORM | renderableType
            );
        }

        _pCurrentScene->lateUpdate();
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
