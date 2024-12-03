#pragma once

#include "Pesukarhu/ecs/systems/System.h"
#include <cstdint>


namespace pk
{
    class BlinkerSystem : public System
    {
    private:
        uint64_t _requiredMask = 0;
        char _blinkChar = '|';
        float _maxBlinkCooldown = 0.5f;

    public:
        BlinkerSystem();
        ~BlinkerSystem();

        void update(Scene* pScene);
    };
}
