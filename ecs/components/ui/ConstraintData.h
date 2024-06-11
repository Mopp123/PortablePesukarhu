#pragma once

#include "ecs/components/Component.h"

namespace pk
{

    enum ConstraintType
    {
        PIXEL_LEFT = 1,
        PIXEL_RIGHT = 2,
        PIXEL_TOP = 3,
        PIXEL_BOTTOM = 4,
        PIXEL_CENTER_HORIZONTAL = 5,
        PIXEL_CENTER_VERTICAL = 6
    };

    class ConstraintData : public Component
    {
    public:
        // The transform component's id this constraint affects
        uint32_t transformID = 0;
        ConstraintType constraintType = PIXEL_LEFT;
        float value = 0.0f;

        ConstraintData(uint32_t transformID, ConstraintType constraintType, float value) :
            Component(PK_UI_CONSTRAINT),
            transformID(transformID),
            constraintType(constraintType),
            value(value)
        {}
    };
}
