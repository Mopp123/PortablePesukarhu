#pragma once

#include <cstddef>

namespace pk
{
    class MemoryPool
    {
    private:
        size_t _size = 0;
        size_t _occupiedSize = 0;
        void* _pStorage = nullptr;

    public:
        MemoryPool() {}
        MemoryPool(size_t size);
        MemoryPool(const MemoryPool& other);
        ~MemoryPool();

        // Allocates chunk of size "size" in pool and returns ptr to that
        void* alloc(size_t size);

        // Clears storage but doesn't resize the actual storage
        // (sets all data and _occupeidSize to 0)
        void clearStorage();
        // Calls free for _pStorage and sets it to nullptr
        void freeStorage();
        void addSpace(size_t newSize);

        inline size_t getSize() const { return _occupiedSize; }
        inline size_t getCapacity() const { return _size; }

        inline void* accessStorage() { return _pStorage; }
    };
}
