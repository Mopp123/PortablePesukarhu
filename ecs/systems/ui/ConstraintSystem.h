#pragma once

#include "../System.h"


namespace pk
{
    namespace ui
    {
        class ConstraintSystem : public System
        {
        public:
            ConstraintSystem();
            ~ConstraintSystem();

            void update();
        };
    }
}
