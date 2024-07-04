#pragma once

#include "core/Memory.h"
#include "ecs/Entity.h"
#include "Component.h"
#include <vector>


namespace pk
{
    // Component memory pool can store only a single type of components
    class ComponentPool : public MemoryPool
    {
    private:
        size_t _componentSize = 0;
        size_t _componentCapacity = 0;
        size_t _componentCount = 0;

        std::vector<size_t> _freeOffsets;

    public:
        ComponentPool(size_t componentSize, size_t componentCapacity);
        ~ComponentPool();

        void* allocComponent();
        void destroyComponent(entityID_t entityID);
        // Atm doesnt check if invalid index!
        void* getComponent_DANGER(entityID_t entityID);
    };
}
