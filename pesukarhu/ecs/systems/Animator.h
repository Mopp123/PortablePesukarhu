#pragma once

#include "System.h"


namespace pk
{
    class Animator : public System
    {
    public:
        Animator();
        ~Animator();

        virtual void update(Scene* pScene);
    };
}
