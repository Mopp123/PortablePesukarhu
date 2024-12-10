#pragma once

#include "pesukarhu/ecs/components/Component.h"
#include "pesukarhu/ecs/Entity.h"

// Affects entity's TextRenderable so when active theres added one char
// which blinks at the end of the text
namespace pk
{
    class Blinker : public Component
    {
    public:
        float blinkCooldown = 0.0f;
        bool blinkState = false;
        bool enable = false;

        Blinker();
        Blinker(const Blinker& other);
        Blinker& operator=(Blinker& other);
    	~Blinker() {}

        static Blinker* create(entityID_t target);
    };
}
