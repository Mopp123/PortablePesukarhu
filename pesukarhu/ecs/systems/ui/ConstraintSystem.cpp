#include "ConstraintSystem.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/Scene.h"
#include "pesukarhu/core/Window.h"
#include "pesukarhu/ecs/components/Transform.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include <utility>


namespace pk
{
    namespace ui
    {
        ConstraintSystem::ConstraintSystem()
        {}

        ConstraintSystem::~ConstraintSystem()
        {}

        // Not tested, propably fucks up?
        void ConstraintSystem::update(Scene* pScene)
        {
            const Window* const win = Application::get()->getWindow();
            const float windowWidth = win->getWidth();
            const float windowHeight = win->getHeight();

            ComponentPool& constraintPool = pScene->componentPools[ComponentType::PK_UI_CONSTRAINT];
            ComponentPool& transformPool = pScene->componentPools[ComponentType::PK_TRANSFORM];

            uint64_t requiredMask = ComponentType::PK_UI_CONSTRAINT | ComponentType::PK_TRANSFORM;
            for (Entity e : pScene->entities)
            {
                if ((e.componentMask & requiredMask) == requiredMask)
                {
                    ConstraintData* pConstraint = (ConstraintData*)constraintPool[e.id];
                    Transform* pTransform = (Transform*)transformPool[e.id];
                    // NOTE: we dont care if these components are inactive here..
                    // TODO: Maybe some parenting system?
                    // -> make child entities' constraints take parent into account!

                    mat4& tMat = pTransform->accessTransformationMatrix();

                    const float& transformWidth = tMat[0 + 0 * 4];
                    const float& transformHeight = tMat[1 + 1 * 4];

                    HorizontalConstraintType horizontalType = pConstraint->horizontalType;
                    float horizontalVal = pConstraint->horizontalValue;

                    VerticalConstraintType verticalType = pConstraint->verticalType;
                    float verticalVal = pConstraint->verticalValue;

                    switch (horizontalType)
                    {
                        case HorizontalConstraintType::PIXEL_LEFT:		        tMat[0 + 3 * 4] = horizontalVal; break;
                        case HorizontalConstraintType::PIXEL_RIGHT:		        tMat[0 + 3 * 4] = windowWidth + horizontalVal; break;
                        case HorizontalConstraintType::PIXEL_CENTER_HORIZONTAL:	tMat[0 + 3 * 4] = windowWidth * 0.5f + horizontalVal; break;
                        default:
                            break;
                    }

                    switch (verticalType)
                    {
                        case VerticalConstraintType::PIXEL_BOTTOM:		        tMat[1 + 3 * 4] = verticalVal; break;
                        case VerticalConstraintType::PIXEL_TOP:			tMat[1 + 3 * 4] = windowHeight + verticalVal;	break;
                        case VerticalConstraintType::PIXEL_CENTER_VERTICAL:		tMat[1 + 3 * 4] = windowHeight * 0.5f + verticalVal; break;
                        default:
                            break;
                    }
                }
            }
        }
    }
}
