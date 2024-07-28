#pragma once

#include "ecs/components/Component.h"
#include "utils/ID.h"


namespace pk
{
    class AnimationData : public Component
    {
    private:
        PK_id _animationResourceID = 0;
        // Indices to animation resource's poses
        uint32_t _currentPose = 0;
        uint32_t _nextPose = 0;
        // Progression between current and next pose
        float _progress = 0.0f;

    public:
        AnimationData(PK_id animationResourceID) :
            Component(ComponentType::PK_ANIMATION_DATA),
            _animationResourceID(animationResourceID)
        {}
        AnimationData(const AnimationData& other) :
            _animationResourceID(other._animationResourceID),
            _currentPose(other._currentPose),
            _nextPose(other._nextPose),
            _progress(other._progress)
        {}
        ~AnimationData();

        inline PK_id getResourceID() const { return _animationResourceID; }
        inline uint32_t getCurrentPose() const { return _currentPose; }
        inline uint32_t getNextPose() const { return _nextPose; }
        inline float getProgress() const { return _progress; }
    };
}
