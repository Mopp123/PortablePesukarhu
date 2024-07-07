#include "Component.h"
#include "utils/ID.h"

namespace pk
{
    Component::Component()
    {}

    Component::Component(ComponentType type) :
        _type(type)
    {
        _id = ID::generate();
    }

    Component::Component(const Component& other) :
        _id(other._id),
        _type(other._type)
    {
    }

    Component::~Component()
    {
        ID::erase(_id);
    }
}
