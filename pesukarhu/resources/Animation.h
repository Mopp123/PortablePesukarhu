#pragma once

#include "Resource.h"
#include "pesukarhu/graphics/animation/Pose.h"
#include <vector>


namespace pk
{
    class Animation : public Resource
    {
    private:
        Pose _bindPose;
        std::vector<Pose> _poses;

    public:
        Animation(const Pose& bindPose, const std::vector<Pose>& poses);
        ~Animation();

        void load() {};
        void save() {};

        inline const Pose& getPose(int keyframe) const { return _poses[keyframe]; }
        inline size_t getKeyframeCount() const { return _poses.size(); }
        inline const Pose& getBindPose() const { return _bindPose; }
    };
}
