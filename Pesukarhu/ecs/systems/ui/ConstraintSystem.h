#pragma once

#include "Pesukarhu/ecs/systems/System.h"


namespace pk
{
    namespace ui
    {
        class ConstraintSystem : public System
        {
        public:
            ConstraintSystem();
            ~ConstraintSystem();

            void update(Scene* pScene);
        };
    }
}
