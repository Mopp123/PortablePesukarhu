#include "ComponentPool.h"
#include "Common.h"
#include "core/Debug.h"

#include "Transform.h"
#include "Camera.h"
#include "ui/ConstraintData.h"

#include "renderable/GUIRenderable.h"
#include "renderable/Sprite3DRenderable.h"
#include "renderable/Static3DRenderable.h"
#include "renderable/TerrainTileRenderable.h"
#include "renderable/TextRenderable.h"
#include "renderable/GUIRenderable.h"

namespace pk
{
    ComponentPool::ComponentPool(const ComponentPool& other) :
        _componentSize(other._componentSize),
        _componentCapacity(other._componentCapacity),
        _componentCount(other._componentCount),
        _allowResize(other._allowResize)
    {
    }

    ComponentPool::ComponentPool(
        size_t componentSize,
        size_t componentCapacity,
        bool allowResize
    ) :
        MemoryPool(componentSize * componentCapacity),
        _componentSize(componentSize),
        _componentCapacity(componentCapacity),
        _allowResize(allowResize)
    {}

    ComponentPool::~ComponentPool()
    {
    }

    ComponentPool::iterator ComponentPool::begin()
    {
        return { _pStorage, _componentSize, 0 };
    }

    ComponentPool::iterator ComponentPool::end()
    {
        return { nullptr, _componentSize, _occupiedSize };
    }

    void* ComponentPool::allocComponent(entityID_t entityID)
    {
        if (_occupiedSize + _componentSize > _totalSize)
        {
            if (!_allowResize)
            {
                Debug::log(
                    "@ComponentPool::allocComponent "
                    "Pool with resizing disabled was already full!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
            else
            {
                addSpace(_totalSize + _componentSize);
                ++_componentCapacity;
            }
        }
        void* ptr = nullptr;
        size_t allocationOffset = _componentCount;
        if (_freeOffsets.empty())
        {
            ptr = alloc(_componentSize);
        }
        else
        {
            allocationOffset = _freeOffsets.back();
            ptr = alloc(_componentSize * allocationOffset, _componentSize);
            _freeOffsets.pop_back();
        }
        if (!ptr)
        {
            Debug::log(
                "@ComponentPool::allocComponent "
                "Failed to allocate from memory pool",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        _componentCount++;
        _entityOffsetMapping[entityID] = allocationOffset;
        return ptr;
    }

    void ComponentPool::destroyComponent(entityID_t entityID)
    {
        if (_entityOffsetMapping.find(entityID) != _entityOffsetMapping.end())
        {
            size_t offset = _entityOffsetMapping[entityID];
            clearStorage(_componentSize * offset, _componentSize);
            _freeOffsets.push_back(offset);
            _entityOffsetMapping.erase(entityID);
            --_componentCount;
        }
        else
        {
            Debug::log(
                "@ComponentPool::destroyComponent "
                "Invalid entityID: " + std::to_string(entityID),
                Debug::MessageType::PK_FATAL_ERROR
            );
        }
    }

    // NOT TESTED, MAY FUK UP!
    void* ComponentPool::getComponent_DANGER(entityID_t entityID)
    {
        size_t offset = _entityOffsetMapping[entityID];
        return ((uint8_t*)_pStorage) + (_componentSize * offset);
    }

    const void * const ComponentPool::getComponent_DANGER(entityID_t entityID) const
    {
        std::unordered_map<entityID_t, size_t>::const_iterator it = _entityOffsetMapping.find(entityID);
        if (it == _entityOffsetMapping.end())
        {
            Debug::log(
                "@ComponentPool::getComponent_DANGER "
                "Failed to find component for entity: " + std::to_string(entityID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        size_t offset = it->second;
        return ((uint8_t*)_pStorage) + (_componentSize * offset);
    }

    void* ComponentPool::operator[](entityID_t entityID)
    {
        size_t offset = _entityOffsetMapping[entityID];
        return (void*)(((PK_byte*)_pStorage) + offset * _componentSize);
    }
}
