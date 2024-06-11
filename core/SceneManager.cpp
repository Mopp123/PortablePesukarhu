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
        // for (System* system : _pCurrentScene->systems[SystemType::PK_SYSTEM_TYPE_UPDATEABLE])
        //     ((Updateable*)system)->update();
        for (System* system : _pCurrentScene->systems[SystemType::PK_SYSTEM_TYPE_UPDATEABLE])
            ((Updateable*)system)->update();

        // NOTE: Deprecated old below!!!
        // Submit all "renderable components" for rendering...

        // TODO:
        // * Get application's master renderer
        // * Get master renderer's renderers
        // * Submit renderable components into those
        Application* pApp = Application::get();
        MasterRenderer* pMasterRenderer = pApp->getMasterRenderer();
        // find gui renderers
        Renderer* pGuiRenderer = pMasterRenderer->getRenderer(ComponentType::PK_RENDERABLE_GUI);
        Renderer* pFontRenderer = pMasterRenderer->getRenderer(ComponentType::PK_RENDERABLE_TEXT);
        // find 3d renderers
        Renderer* pTerrainRenderer = pMasterRenderer->getRenderer(ComponentType::PK_RENDERABLE_TERRAINTILE);
        Renderer* pSpriteRenderer = pMasterRenderer->getRenderer(ComponentType::PK_RENDERABLE_SPRITE3D);


        // submitting renderables

        // TERRAIN TILES
        //*atm we want to pass just empty tMatrix, since tile figures out its' vertex positions from the tile component itself
        mat4 empty;
        for (const Component* const c_renderableTile : _pCurrentScene->getComponentsOfTypeInScene(ComponentType::PK_RENDERABLE_TERRAINTILE))
        {
            if (c_renderableTile->isActive())
            {
                pTerrainRenderer->submit(c_renderableTile, empty);
            }
        }
        // 3D SPRITES
        for (const Component* const c_renderableSprite : _pCurrentScene->getComponentsOfTypeInScene(ComponentType::PK_RENDERABLE_SPRITE3D))
        {
            if (c_renderableSprite->isActive())
            {
                pSpriteRenderer->submit(c_renderableSprite, empty);
            }
        }

        // GUI
        for (const Component * const c_renderableGUI : _pCurrentScene->getComponentsOfTypeInScene(ComponentType::PK_RENDERABLE_GUI))
        {
            Component* rawTransform = _pCurrentScene->getComponent(c_renderableGUI->getEntity(), ComponentType::PK_TRANSFORM);
            if (c_renderableGUI->isActive())
            {
                if (rawTransform)
                {
                    Transform* transform = (Transform*)rawTransform;
                    pGuiRenderer->submit(c_renderableGUI, transform->getTransformationMatrix());
                }
            }
        }
        // TEXT
        for (const Component* const c_renderableText : _pCurrentScene->getComponentsOfTypeInScene(ComponentType::PK_RENDERABLE_TEXT))
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
            Debug::log("Attempting to switch scene...");
            Application::get()->accessInputManager()->destroyEvents();
            delete _pCurrentScene;
            _pCurrentScene = _pNextScene;
            _pCurrentScene->init();
            _pNextScene = nullptr;
        }
    }
}
