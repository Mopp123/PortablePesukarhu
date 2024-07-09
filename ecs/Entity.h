#pragma once

#include <cstdint>

#define NULL_ENTITY_ID -1
typedef int64_t entityID_t;


namespace pk
{
    struct Entity
    {
        entityID_t id = -1;
        uint64_t componentMask = 0;

        Entity() {}
        Entity(const Entity& other) :
            id(other.id),
            componentMask(other.componentMask)
        {}
    };
}
