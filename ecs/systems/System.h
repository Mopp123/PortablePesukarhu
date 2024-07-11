#pragma once

#include "ecs/components/Component.h"
#include <vector>


namespace pk
{
    class System
    {
    public:
        System() = default;
        System(const System& other) = delete;
        virtual ~System() {}

        virtual void update() = 0;
    };
}
