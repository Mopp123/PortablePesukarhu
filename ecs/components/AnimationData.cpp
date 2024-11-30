#include "AnimationData.h"
#include "core/Application.h"


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

    AnimationData::~AnimationData()
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

    AnimationData* AnimationData::create(
        entityID_t target,
        PK_id animationResourceID,
        AnimationMode mode,
        float speed,
        std::vector<uint32_t> keyframes
    )
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        AnimationData* pComponent = (AnimationData*)pScene->componentPools[ComponentType::PK_ANIMATION_DATA].allocComponent(target);
        // For now figure out joint count here.. maybe in future somewhere else..
        ResourceManager& resManager = Application::get()->getResourceManager();
        const Animation* pAnimResource = (const Animation*)resManager.getResource(animationResourceID);
        if (!pAnimResource)
        {
            Debug::log(
                "@AnimationData::create "
                "Failed to find AnimationResource with ID: " + std::to_string(animationResourceID),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        *pComponent = AnimationData(
            animationResourceID,
            mode,
            speed,
            pAnimResource->getBindPose(),
            keyframes
        );
        pScene->addComponent(target, pComponent);
        return pComponent;
    }
}
