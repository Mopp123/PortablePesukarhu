#include "Memory.h"
#include <cstring>
#include <cstdlib>
#include "Debug.h"
#include "pesukarhu/Common.h"


namespace pk
{
     MemoryPool::MemoryPool(size_t capacity) :
        _totalSize(capacity),
        _occupiedSize(0)
    {
        _pStorage = calloc(_totalSize, 1);
        memset(_pStorage, 0, _totalSize);
    }

    MemoryPool::MemoryPool(const MemoryPool& other) :
        _totalSize(other._totalSize),
        _occupiedSize(other._occupiedSize),
        _pStorage(other._pStorage)
    {
        Debug::log(
            "Copied MemoryPool!",
            Debug::MessageType::PK_WARNING
        );
    }

    MemoryPool::~MemoryPool()
    {
    }

    void* MemoryPool::alloc(size_t size)
    {
        if (_occupiedSize + size > _totalSize)
        {
            Debug::log(
                "@MemoryPool::alloc(1) Capacity exceeded!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        void* ptr = ((PK_byte*)_pStorage) + _occupiedSize;
        _occupiedSize += size;
        return ptr;
    }

    void* MemoryPool::alloc(size_t offset, size_t size)
    {
        if (offset + size > _totalSize)
        {
            Debug::log(
                "@MemoryPool::alloc(2) Capacity exceeded!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        void* ptr = ((PK_byte*)_pStorage) + offset;
        _occupiedSize += size;
        return ptr;
    }

    void MemoryPool::clearStorage()
    {
        memset(_pStorage, 0, _totalSize);
        _occupiedSize = 0;
    }

    void MemoryPool::clearStorage(size_t offset, size_t size)
    {
        // If clearing from current "back" pos
        //  -> decreace _occupiedSize so next allocation will reside
        //  at the freed "back"
        if (offset + size  == _occupiedSize)
            _occupiedSize -= size;
        memset(((PK_byte*)_pStorage) + offset, 0, size);
    }

    void MemoryPool::freeStorage()
    {
        free(_pStorage);
        _pStorage = nullptr;
        _totalSize = 0;
        _occupiedSize = 0;
    }

    void MemoryPool::addSpace(size_t newSize)
    {
        if (newSize < _totalSize)
        {
            Debug::log(
                "@MemoryPool::addSpace newSize was less than current size",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }

        void* pNewStorage = calloc(newSize, 1);
        memset(pNewStorage, 0, newSize);

        memcpy(pNewStorage, _pStorage, _totalSize);

        free(_pStorage);
        _pStorage = pNewStorage;
        Debug::log(
            "@MemoryPool::addSpace Increased size from " + std::to_string(_totalSize) + " to " + std::to_string(newSize)
        );
        _totalSize = newSize;
    }
}
