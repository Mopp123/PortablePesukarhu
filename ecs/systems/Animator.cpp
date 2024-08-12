#include "Animator.h"
#include "core/Application.h"
#include "core/Scene.h"
#include "ecs/components/ComponentPool.h"


namespace pk
{
    static void set_pose(
        Scene& scene,
        entityID_t jointEntity,
        const Pose& pose,
        int currentJointIndex
    )
    {
        Transform* pTransform = (Transform*)scene.getComponent(jointEntity, ComponentType::PK_TRANSFORM);
        const Joint& joint = pose.joints[currentJointIndex];
        pTransform->setPos(joint.translation);
        pTransform->setRotation(joint.rotation);

        std::vector<entityID_t> childJointEntities = scene.getChildren(jointEntity);
        for (int i = 0; i < childJointEntities.size(); ++i)
        {
            int childJointIndex = pose.jointChildMapping[currentJointIndex][i];
            set_pose(scene, childJointEntities[i], pose, childJointIndex);
        }
    }


    static void apply_interpolation_to_joints(
        Scene& scene,
        entityID_t jointEntity,
        const Pose& current,
        const Pose& next,
        float amount,
        int currentJointIndex
    )
    {
        Transform* pTransform = (Transform*)scene.getComponent(jointEntity, ComponentType::PK_TRANSFORM);
        const Joint& jointCurrentPose = current.joints[currentJointIndex];
        const Joint& jointNextPose = next.joints[currentJointIndex];

        // TODO: Fix anim translations? something wonky 'bout those..
        vec3 interpolatedTranslation = jointCurrentPose.translation.lerp(jointNextPose.translation, amount);
        quat interpolatedRotation = jointCurrentPose.rotation.slerp(jointNextPose.rotation, amount);
        pTransform->setPos(interpolatedTranslation);
        pTransform->setRotation(interpolatedRotation);

        std::vector<entityID_t> childJointEntities = scene.getChildren(jointEntity);
        for (int i = 0; i < childJointEntities.size(); ++i)
        {
            int childJointIndex = current.jointChildMapping[currentJointIndex][i];
            apply_interpolation_to_joints(scene, childJointEntities[i], current, next, amount, childJointIndex);
        }
    }


    Animator::Animator()
    {}

    Animator::~Animator()
    {}

    // TODO: Optimize below!
    void Animator::update(Scene* pScene)
    {
        const ResourceManager& resourceManager = Application::get()->getResourceManager();
        ComponentPool& transformPool = pScene->componentPools[ComponentType::PK_TRANSFORM];
        ComponentPool& animDataPool = pScene->componentPools[ComponentType::PK_ANIMATION_DATA];

        uint64_t requiredMask = ComponentType::PK_ANIMATION_DATA | ComponentType::PK_TRANSFORM;
        for (Entity e : pScene->entities)
        {
            if ((e.componentMask & requiredMask) == requiredMask)
            {
                AnimationData* pAnimData = (AnimationData*)animDataPool[e.id];
                // Override stopped if mode changed elsewhere
                if (pAnimData->_mode == AnimationMode::PK_ANIMATION_MODE_REPEAT)
                    pAnimData->_stopped = false;

                if (pAnimData->_stopped)
                    continue;

                Transform* pTransform = (Transform*)transformPool[e.id];
                const Animation* pAnimationResource = (const Animation*)resourceManager.getResource(pAnimData->getResourceID());
                if (!pAnimationResource)
                {
                    Debug::log(
                        "@Animator::update "
                        "Failed to find AnimationData component's animation resource! "
                        "Entity id: " + std::to_string(e.id) + " "
                        "Animation resource id: " + std::to_string(pAnimData->getResourceID()),
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    continue;
                }

                // TODO:
                // Fucking overly complicated below -> clean that up!!
                const size_t keyframeCount = pAnimationResource->getKeyframeCount();
                const AnimationMode& animMode = pAnimData->_mode;

                // Expected to have AnimationData component on the root joint entity
                uint32_t& currentPoseIndex = pAnimData->_currentPose;
                uint32_t& nextPoseIndex = pAnimData->_nextPose;
                const Pose& currentPose = pAnimationResource->getPose(currentPoseIndex);
                const Pose& nextPose = pAnimationResource->getPose(pAnimData->_nextPose);

                apply_interpolation_to_joints(
                    *pScene,
                    e.id,
                    currentPose,
                    nextPose,
                    pAnimData->_progress,
                    0
                );

                const float TEST_animSpeed = 1.0f;
                pAnimData->_progress += TEST_animSpeed * Timing::get_delta_time();
                if (pAnimData->_progress >= 1.0f)
                {
                    pAnimData->_progress = 0.0f;

                    currentPoseIndex += 1;
                    nextPoseIndex += 1;

                    if (nextPoseIndex >= keyframeCount && animMode == AnimationMode::PK_ANIMATION_MODE_PLAY_ONCE)
                            pAnimData->_stopped = true;

                    currentPoseIndex %= keyframeCount;
                    nextPoseIndex %= keyframeCount;
                }
            }
        }
    }
}
