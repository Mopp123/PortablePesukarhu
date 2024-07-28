#include "Animation.h"

namespace pk
{
    Animation::Animation(const std::vector<Pose>& poses) :
        Resource(ResourceType::RESOURCE_ANIMATION),
        _poses(poses)
    {}

    Animation::~Animation()
    {}
}
