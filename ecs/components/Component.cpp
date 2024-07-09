#include "Component.h"


namespace pk
{
    Component::Component()
    {}

    Component::Component(ComponentType type) :
        _type(type)
    {
    }

    Component::Component(const Component& other) :
        _id(other._id),
        _type(other._type)
    {
    }

    Component::~Component()
    {
    }
}
