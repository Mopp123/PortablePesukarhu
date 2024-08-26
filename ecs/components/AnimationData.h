#pragma once

#include "ecs/components/Component.h"
#include "utils/ID.h"
#include "graphics/animation/Pose.h"


namespace pk
{
    class Animator;


    enum AnimationMode
    {
        PK_ANIMATION_MODE_PLAY_ONCE,
        PK_ANIMATION_MODE_REPEAT
    };


    class AnimationData : public Component
    {
    private:
        friend class Animator;

        PK_id _animationResourceID = 0;
        AnimationMode _mode = AnimationMode::PK_ANIMATION_MODE_PLAY_ONCE;
        // Indices to animation resource's poses
        uint32_t _currentPose = 0;
        uint32_t _nextPose = 1;
        // Progression between current and next pose
        float _progress = 0.0f;
        float _speed = 1.0f;

        bool _stopped = false;

        Pose _resultPose;

    public:
        AnimationData(PK_id animationResourceID, AnimationMode mode, float speed, const Pose& bindPose) :
            Component(ComponentType::PK_ANIMATION_DATA),
            _animationResourceID(animationResourceID),
            _mode(mode),
            _speed(speed),
            _resultPose(bindPose)
        {
        }
        AnimationData(const AnimationData& other) :
            _animationResourceID(other._animationResourceID),
            _mode(other._mode),
            _currentPose(other._currentPose),
            _nextPose(other._nextPose),
            _progress(other._progress),
            _resultPose(other._resultPose)
        {}
        ~AnimationData() {}

        inline PK_id getResourceID() const { return _animationResourceID; }
        inline uint32_t getCurrentPose() const { return _currentPose; }
        inline uint32_t getNextPose() const { return _nextPose; }
        inline float getProgress() const { return _progress; }
        inline void setProgress(float progress) { _progress = progress; }

        inline void setMode(AnimationMode mode) { _mode = mode; }
        inline float getSpeed() const { return _speed; }

        inline void TEST_setCurrentPose(uint32_t keyframe) { _currentPose = keyframe; }
        inline void TEST_setNextPose(uint32_t keyframe) { _nextPose = keyframe; }

        inline void setResultPoseJoint(const Joint& joint, int jointIndex) { _resultPose.joints[jointIndex] = joint; }
        inline const Pose& getResultPose() const { return _resultPose; }
    };
}
