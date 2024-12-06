#pragma once

#include "pesukarhu/core/Memory.h"
#include "pesukarhu/ecs/Entity.h"
#include "Component.h"
#include <vector>
#include <unordered_map>


namespace pk
{

    // Component memory pool can store only a single type of components
    class ComponentPool : public MemoryPool
    {
    private:
        size_t _componentSize = 0;
        size_t _componentCapacity = 0;
        size_t _componentCount = 0;
        bool _allowResize = false;

        std::vector<size_t> _freeOffsets;
        // Actual offset of entity's component in storage is determined by this
        // NOTE: VERY IMPORTANT! Here when we talk about offsets we mean
        // component index and not the actual byte offset in _pStorage
        // unlike in MemoryPool!
        std::unordered_map<entityID_t, size_t> _entityOffsetMapping;
    public:
        struct iterator
        {
            void* ptr;
            size_t componentSize = 0;
            size_t position = 0;

            bool operator!=(const iterator& other)
            {
                return position != other.position;
            }

            iterator& operator++()
            {
                ptr = ((uint8_t*)ptr) + componentSize;
                position += componentSize;
                return *this;
            }
        };

    public:
        ComponentPool() {}
        ComponentPool(const ComponentPool& other);
        ComponentPool(size_t componentSize, size_t componentCapacity, bool allowResize);
        ~ComponentPool();

        iterator begin();
        iterator end();


        // If allowResize and not enough space, this also adds space to the _pStorage
        void* allocComponent(entityID_t entityID);
        void destroyComponent(entityID_t entityID);
        // Atm doesnt check if invalid index!
        void* getComponent_DANGER(entityID_t entityID);
        const void * const getComponent_DANGER(entityID_t entityID) const;

        void* operator[](entityID_t entityID);
    };
}
