#include "ConstraintData.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    ConstraintData::ConstraintData(ConstraintProperties properties) :
        Component(PK_UI_CONSTRAINT),
        horizontalType(properties.horizontalType),
        horizontalValue(properties.horizontalValue),
        verticalType(properties.verticalType),
        verticalValue(properties.verticalValue)
    {}

    ConstraintData* ConstraintData::create(entityID_t target, ConstraintProperties properties)
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        ConstraintData* pConstraint = (ConstraintData*)pScene->componentPools[ComponentType::PK_UI_CONSTRAINT].allocComponent(target);
        *pConstraint = ConstraintData(properties);
        pScene->addComponent(target, pConstraint);
        return pConstraint;
    }
}
