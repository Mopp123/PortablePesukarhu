#include "Memory.h"
#include <cstring>
#include <cstdlib>
#include "Debug.h"
#include "Common.h"


namespace pk
{
     MemoryPool::MemoryPool(size_t size) :
        _size(size),
        _occupiedSize(0)
    {
        _pStorage = calloc(_size, 1);
        memset(_pStorage, 0, _size);
    }

    MemoryPool::MemoryPool(const MemoryPool& other) :
        _size(other._size),
        _occupiedSize(other._occupiedSize),
        _pStorage(other._pStorage)
    {
        Debug::log(
            "Copied MemoryPool! (This is fine if done only by ResourceManager)",
            Debug::MessageType::PK_WARNING
        );
    }

    MemoryPool::~MemoryPool()
    {
    }

    void* MemoryPool::alloc(size_t size)
    {
        if (_occupiedSize + size > _size)
        {
            Debug::log(
                "@MemoryPool::alloc Capacity exceeded!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        void* ptr = ((PK_byte*)_pStorage) + _occupiedSize;
        _occupiedSize += size;
        return ptr;
    }

    void MemoryPool::clearStorage()
    {
        memset(_pStorage, 0, _size);
        _occupiedSize = 0;
    }

    void MemoryPool::freeStorage()
    {
        free(_pStorage);
        _pStorage = nullptr;
        _size = 0;
        _occupiedSize = 0;
    }

    void MemoryPool::addSpace(size_t newSize)
    {
        if (newSize < _size)
        {
            Debug::log(
                "@MemoryPool::addSpace newSize was less than current size",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }

        void* pNewStorage = calloc(newSize, 1);
        memset(pNewStorage, 0, newSize);

        memcpy(pNewStorage, _pStorage, _size);

        free(_pStorage);
        _pStorage = pNewStorage;
        _size = newSize;
    }
}
