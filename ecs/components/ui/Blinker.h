#pragma once

#include "ecs/components/Component.h"
#include <unordered_map>
#include "ecs/Entity.h"
#include <memory>

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
    };
}
