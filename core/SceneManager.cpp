#include "SceneManager.h"
#include "Debug.h"
#include "Application.h"
#include "ecs/components/renderable/GUIRenderable.h"
#include "../ecs/components/Transform.h"


namespace pk
{
    void SceneManager::handleSceneUpdate()
    {
        _pCurrentScene->update();

        // Update all systems of the scene
        for (System* system : _pCurrentScene->systems)
            system->update();

        // Submit all "renderable components" for rendering...

        Application* pApp = Application::get();
        MasterRenderer* pMasterRenderer = pApp->getMasterRenderer();
        // find gui renderers
        Renderer* pGuiRenderer = pMasterRenderer->getRenderer(ComponentType::PK_RENDERABLE_GUI);
        Renderer* pFontRenderer = pMasterRenderer->getRenderer(ComponentType::PK_RENDERABLE_TEXT);
        // find 3d renderers
        Renderer* pTerrainRenderer = pMasterRenderer->getRenderer(ComponentType::PK_RENDERABLE_TERRAINTILE);
        Renderer* pSpriteRenderer = pMasterRenderer->getRenderer(ComponentType::PK_RENDERABLE_SPRITE3D);
        Renderer* pStaticRenderer = pMasterRenderer->getRenderer(ComponentType::PK_RENDERABLE_STATIC3D);


        // submitting renderables

        // TERRAIN TILES
        //*atm we want to pass just empty tMatrix, since tile figures out its' vertex positions from the tile component itself
        //mat4 empty;
        //for (const Component* const c_renderableTile : _pCurrentScene->getComponentsOfTypeInScene(ComponentType::PK_RENDERABLE_TERRAINTILE))
        //{
        //    if (c_renderableTile->isActive())
        //    {
        //        pTerrainRenderer->submit(c_renderableTile, empty);
        //    }
        //}
        //// 3D SPRITES
        //for (const Component* const c_renderableSprite : _pCurrentScene->getComponentsOfTypeInScene(ComponentType::PK_RENDERABLE_SPRITE3D))
        //{
        //    if (c_renderableSprite->isActive())
        //    {
        //        pSpriteRenderer->submit(c_renderableSprite, empty);
        //    }
        //}

        //// GUI
        // BOTTLENECK!
        // Atm testing accessing renderable through mem pool

        ComponentPool& transformPool = _pCurrentScene->componentPools[ComponentType::PK_TRANSFORM];

        ComponentPool& renderableGUIPool = _pCurrentScene->componentPools[ComponentType::PK_RENDERABLE_GUI];
        for (const Entity& e : _pCurrentScene->entities)
        {
            if (e.componentMask & ComponentType::PK_TRANSFORM &&
                e.componentMask & ComponentType::PK_RENDERABLE_GUI)
            {
                GUIRenderable* pRenderable = (GUIRenderable*)renderableGUIPool[e.id];
                Transform* pTransform = (Transform*)transformPool[e.id];
                if (!pRenderable->isActive())
                    continue;

                pGuiRenderer->submit(pRenderable, pTransform->getTransformationMatrix());
            }
        }
        // TEXT
        ComponentPool& renderableTextPool = _pCurrentScene->componentPools[ComponentType::PK_RENDERABLE_TEXT];
        for (const Entity& e : _pCurrentScene->entities)
        {
            if (e.componentMask & ComponentType::PK_TRANSFORM &&
                e.componentMask & ComponentType::PK_RENDERABLE_TEXT)
            {
                TextRenderable* pRenderable = (TextRenderable*)renderableTextPool[e.id];
                Transform* pTransform = (Transform*)transformPool[e.id];
                if (!pRenderable->isActive())
                    continue;

                pFontRenderer->submit(pRenderable, pTransform->getTransformationMatrix());
            }
        }
        // Static 3D
        ComponentPool& staticRenderablePool  = _pCurrentScene->componentPools[ComponentType::PK_RENDERABLE_STATIC3D];
        for (const Entity& e : _pCurrentScene->entities)
        {
            if (e.componentMask & ComponentType::PK_TRANSFORM &&
                e.componentMask & ComponentType::PK_RENDERABLE_STATIC3D)
            {
                Static3DRenderable* pRenderable = (Static3DRenderable*)staticRenderablePool[e.id];
                Transform* pTransform = (Transform*)transformPool[e.id];
                if (!pRenderable->isActive())
                    continue;

                pStaticRenderer->submit(pRenderable, pTransform->getTransformationMatrix());
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
            Debug::log("Attempting to switch scene...");
            Application* pApp = Application::get();
            pApp->accessInputManager()->destroyEvents();
            pApp->getResourceManager().free();
            delete _pCurrentScene;
            _pCurrentScene = _pNextScene;
            _pCurrentScene->init();
            _pNextScene = nullptr;
        }
    }
}
