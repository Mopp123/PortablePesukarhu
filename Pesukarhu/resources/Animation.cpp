#include "Animation.h"

namespace pk
{
    Animation::Animation(const Pose& bindPose, const std::vector<Pose>& poses) :
        Resource(ResourceType::RESOURCE_ANIMATION),
        _bindPose(bindPose),
        _poses(poses)
    {}

    Animation::~Animation()
    {}
}
