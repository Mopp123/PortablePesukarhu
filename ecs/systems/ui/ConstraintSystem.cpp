#include "ConstraintSystem.h"
#include "../../../utils/pkmath.h"
#include "../../../core/Application.h"
#include "../../../core/Window.h"
#include "../../components/Transform.h"
#include "ecs/components/ui/ConstraintData.h"
#include <utility>


namespace pk
{
    namespace ui
    {
        ConstraintSystem::ConstraintSystem()
        {}

        ConstraintSystem::~ConstraintSystem()
        {}

        // TODO: Make boundary checks and overall safer...
        void ConstraintSystem::update()
        {
            const Window* const win = Application::get()->getWindow();
            const float windowWidth = win->getWidth();
            const float windowHeight = win->getHeight();

            Scene* pCurrentScene = Application::get()->accessCurrentScene();
            // First find all relevant components..
            std::vector<uint32_t> componentIds;
            std::vector<ConstraintData*> pConstraintComponents;

            if (pCurrentScene->typeComponentMapping.find(ComponentType::PK_UI_CONSTRAINT) != pCurrentScene->typeComponentMapping.end())
            {
                componentIds = pCurrentScene->typeComponentMapping[ComponentType::PK_UI_CONSTRAINT];
                pConstraintComponents.reserve(componentIds.size());
                for (uint32_t componentID : componentIds)
                    pConstraintComponents.emplace_back((ConstraintData*)pCurrentScene->components[componentID]);
            }

            for (ConstraintData* pConstraintComponent : pConstraintComponents)
            {
                Transform* pTransform = (Transform*)(pCurrentScene->components[pConstraintComponent->transformID]);
                mat4& tMat = pTransform->accessTransformationMatrix();

                const float& transformWidth = tMat[0 + 0 * 4];
                const float& transformHeight = tMat[1 + 1 * 4];

                float constraintVal = pConstraintComponent->value;

                switch (pConstraintComponent->constraintType)
                {
                    case ConstraintType::PIXEL_LEFT:		        tMat[0 + 3 * 4] = constraintVal; break;
                    case ConstraintType::PIXEL_RIGHT:		        tMat[0 + 3 * 4] = windowWidth - constraintVal - transformWidth; break;
                    case ConstraintType::PIXEL_BOTTOM:		        tMat[1 + 3 * 4] = constraintVal + transformHeight; break;
                    case ConstraintType::PIXEL_TOP:			tMat[1 + 3 * 4] = windowHeight - constraintVal;	break;
                    case ConstraintType::PIXEL_CENTER_HORIZONTAL:	tMat[0 + 3 * 4] = windowWidth * 0.5f + constraintVal; break;
                    case ConstraintType::PIXEL_CENTER_VERTICAL:		tMat[1 + 3 * 4] = windowHeight * 0.5f + constraintVal; break;
                    default:
                        break;
                }
            }
        }
    }
}
