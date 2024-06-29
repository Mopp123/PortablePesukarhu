#include "Component.h"
#include "utils/ID.h"

namespace pk
{
    Component::Component(ComponentType type) :
        _type(type)
    {
        _id = ID::generate();
    }

    Component::~Component()
    {
        ID::erase(_id);
    }
}
