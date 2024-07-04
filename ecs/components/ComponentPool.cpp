#include "ComponentPool.h"
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
    ComponentPool::ComponentPool(size_t componentSize, size_t componentCapacity) :
        MemoryPool(componentSize * componentCapacity),
        _componentSize(componentSize),
        _componentCapacity(componentCapacity)
    {}

    ComponentPool::~ComponentPool()
    {
    }

    void* ComponentPool::allocComponent()
    {
        void* ptr = alloc(_componentSize);
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
        return ptr;
    }

    void ComponentPool::destroyComponent(entityID_t entityID)
    {
        Debug::notify_unimplemented("ComponentPool<T>::destroyComponent");
    }

    // NOT TESTED, MAY FUK UP!
    void* ComponentPool::getComponent_DANGER(entityID_t entityID)
    {
        return ((uint8_t*)_pStorage) + (_componentSize * entityID);
    }
}
