#pragma once

#include "ecs/components/Component.h"
#include "ecs/Entity.h"


namespace pk
{
    enum HorizontalConstraintType
    {
        PIXEL_LEFT = 1,
        PIXEL_RIGHT = 2,
        PIXEL_CENTER_HORIZONTAL = 3
    };

    enum VerticalConstraintType
    {
        PIXEL_TOP = 3,
        PIXEL_BOTTOM = 4,
        PIXEL_CENTER_VERTICAL = 5
    };

    struct HorizontalConstraintProperties
    {
        HorizontalConstraintType type;
        float value;
    };

    struct VerticalConstraintProperties
    {
        VerticalConstraintType type;
        float value;
    };

    struct ConstraintProperties
    {
        HorizontalConstraintType horizontalType;
        float horizontalValue;
        VerticalConstraintType verticalType;
        float verticalValue;
    };

    class ConstraintData : public Component
    {
    public:
        HorizontalConstraintType horizontalType = PIXEL_LEFT;
        float horizontalValue = 0.0f;
        VerticalConstraintType verticalType = PIXEL_TOP;
        float verticalValue = 0.0f;

        ConstraintData(ConstraintProperties properties);
        static ConstraintData* create(entityID_t target, ConstraintProperties properties);
    };
}
