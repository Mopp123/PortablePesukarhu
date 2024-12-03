#include "BlinkerSystem.h"
#include "Pesukarhu/core/Scene.h"
#include "Pesukarhu/ecs/components/ui/Blinker.h"
#include "Pesukarhu/core/Timing.h"


namespace pk
{
    BlinkerSystem::BlinkerSystem()
    {
        _requiredMask = PK_BLINKER | PK_RENDERABLE_TEXT;
    }

    BlinkerSystem::~BlinkerSystem()
    {}

    void BlinkerSystem::update(Scene* pScene)
    {
        std::unordered_map<ComponentType, ComponentPool>::iterator blinkerPoolIt = pScene->componentPools.find(ComponentType::PK_BLINKER);
        std::unordered_map<ComponentType, ComponentPool>::iterator renderablePoolIt = pScene->componentPools.find(ComponentType::PK_RENDERABLE_TEXT);
        if (blinkerPoolIt == pScene->componentPools.end())
        {
            Debug::log(
                "@BlinkerSystem::update "
                "Scene doesn't have Blinker component pool",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        if (renderablePoolIt == pScene->componentPools.end())
        {
            Debug::log(
                "@BlinkerSystem::update "
                "Scene doesn't have TextRenderable component pool",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        ComponentPool& blinkerPool = blinkerPoolIt->second;
        ComponentPool& renderablePool = renderablePoolIt->second;
        for (Entity e : pScene->entities)
        {
            if ((e.componentMask & _requiredMask) == _requiredMask)
            {
                // ...a bit unsafe?
                Blinker* pBlinker = (Blinker*)blinkerPool[e.id];
                TextRenderable* pRenderable = (TextRenderable*)renderablePool[e.id];
                if (!pBlinker->isActive() || !pBlinker->enable)
                    continue;

                if (pBlinker->blinkState)
                {
                    pRenderable->accessVisualStr() = pRenderable->accessStr() + _blinkChar;
                }
                else
                {
                    pRenderable->accessVisualStr() = pRenderable->accessStr();
                }

                if (pBlinker->blinkCooldown > 0.0f)
                {
                    pBlinker->blinkCooldown -= 1.0f * Timing::get_delta_time();
                }
                else
                {
                    pBlinker->blinkCooldown = _maxBlinkCooldown;
                    pBlinker->blinkState = !pBlinker->blinkState;
                }
            }
        }
    }
}
