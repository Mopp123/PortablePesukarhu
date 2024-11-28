#include "AnimationData.h"

namespace pk
{
    AnimationData::AnimationData(
        PK_id animationResourceID,
        AnimationMode mode,
        float speed,
        const Pose& bindPose
    ) :
        Component(ComponentType::PK_ANIMATION_DATA),
        _animationResourceID(animationResourceID),
        _mode(mode),
        _speed(speed),
        _resultPose(bindPose)
    {
    }

    AnimationData::AnimationData(
        PK_id animationResourceID,
        AnimationMode mode,
        float speed,
        const Pose& bindPose,
        std::vector<uint32_t> keyframes
    ) :
        AnimationData(
            animationResourceID,
            mode,
            speed,
            bindPose
        )
    {
        _keyframes = keyframes;
    }

    AnimationData::AnimationData(const AnimationData& other) :
        _animationResourceID(other._animationResourceID),
        _mode(other._mode),
        _currentPose(other._currentPose),
        _nextPose(other._nextPose),
        _progress(other._progress),
        _speed(other._speed),
        _keyframes(other._keyframes),
        _resultPose(other._resultPose)
    {}

    void AnimationData::reset()
    {
        _progress = 0;
        _currentPose = 0;
        _nextPose = 1;
    }

    void AnimationData::set(
        const std::vector<uint32_t>& keyframes,
        float speed,
        AnimationMode mode
    )
    {
        _keyframes = keyframes;
        _speed = speed;
        reset();
    }
}
