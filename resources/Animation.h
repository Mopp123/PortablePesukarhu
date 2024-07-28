#pragma once

#include "Resource.h"
#include "graphics/animation/Pose.h"
#include <vector>


namespace pk
{
    class Animation : public Resource
    {
    private:
        std::vector<Pose> _poses;

    public:
        Animation(const std::vector<Pose>& poses);
        ~Animation();

        void load() {};
        void save() {};
    };
}
