#include "Blinker.h"

namespace pk
{
    Blinker::Blinker() :
        Component(ComponentType::PK_BLINKER)
    {}

    Blinker::Blinker(const Blinker& other) :
        Component(ComponentType::PK_BLINKER),
        blinkCooldown(other.blinkCooldown),
        blinkState(other.blinkState),
        enable(other.enable)
    {
        _isActive = other._isActive;
    }

    Blinker& Blinker::operator=(Blinker& other)
    {
        std::swap(_type, other._type);
        std::swap(_isActive, other._isActive);
        std::swap(blinkCooldown, other.blinkCooldown);
        std::swap(blinkState, other.blinkState);
        std::swap(enable, other.enable);
        return *this;
    }
}
