#include "Animator.h"
#include "core/Application.h"
#include "core/Scene.h"
#include "ecs/components/ComponentPool.h"


namespace pk
{
    // Was using this when skeleton was hierarchy of Transforms and needed to apply
    // the animation to those..
    static void apply_interpolation_to_joints_FAST(
        Scene& scene,
        AnimationData* pAnimData,
        const Pose& bindPose,
        const Pose& current,
        const Pose& next,
        float amount,
        int currentJointIndex,
        const mat4& parentMatrix
    )
    {
        const Joint& jointCurrentPose = current.joints[currentJointIndex];
        const Joint& jointNextPose = next.joints[currentJointIndex];

        // TODO: Include scaling
        vec3 interpolatedTranslation = jointCurrentPose.translation.lerp(jointNextPose.translation, amount);
        mat4 translationMatrix(1.0f);
        translationMatrix[0 + 3 * 4] = interpolatedTranslation.x;
        translationMatrix[1 + 3 * 4] = interpolatedTranslation.y;
        translationMatrix[2 + 3 * 4] = interpolatedTranslation.z;
        quat interpolatedRotation = jointCurrentPose.rotation.slerp(jointNextPose.rotation, amount);

        const mat4& inverseBindMatrix = bindPose.joints[currentJointIndex].inverseMatrix;
        mat4 m = parentMatrix * translationMatrix * interpolatedRotation.toRotationMatrix();

        pAnimData->setResultPoseJoint(
            {
                interpolatedTranslation,
                interpolatedRotation,
                { 1.0f, 1.0f, 1.0f }, // Atm scaling is disabled!
                m * inverseBindMatrix
            },
            currentJointIndex
        );

        const std::vector<int>& childJoints = bindPose.jointChildMapping[currentJointIndex];
        for (int i = 0; i < childJoints.size(); ++i)
        {
            int childJointIndex = childJoints[i];
            apply_interpolation_to_joints_FAST(
                scene,
                pAnimData,
                bindPose,
                current,
                next,
                amount,
                childJointIndex,
                m
            );
        }
    }


    static void apply_interpolation_to_joints_SLOW(
        Scene& scene,
        AnimationData* pAnimationData,
        const Pose& bindPose,
        const Pose& current,
        const Pose& next,
        float amount,
        int currentJointIndex,
        const mat4& parentMatrix
    )
    {
        const Joint& jointCurrentPose = current.joints[currentJointIndex];
        const Joint& jointNextPose = next.joints[currentJointIndex];

        vec3 interpolatedTranslation = jointCurrentPose.translation.lerp(jointNextPose.translation, amount);
        quat interpolatedRotation = jointCurrentPose.rotation.slerp(jointNextPose.rotation, amount);

        mat4 translationMatrix(1.0f);
        translationMatrix[0 + 3 * 4] = interpolatedTranslation.x;
        translationMatrix[1 + 3 * 4] = interpolatedTranslation.y;
        translationMatrix[2 + 3 * 4] = interpolatedTranslation.z;

        const mat4& inverseBindMatrix = bindPose.joints[currentJointIndex].inverseMatrix;
        mat4 m = parentMatrix * translationMatrix * interpolatedRotation.toRotationMatrix();

        pAnimationData->setResultPoseJoint(
            {
                interpolatedTranslation,
                interpolatedRotation,
                { 1.0f, 1.0f, 1.0f }, // Atm scaling is disabled!
                m * inverseBindMatrix
            },
            currentJointIndex
        );

        for (int i = 0; i < bindPose.jointChildMapping[currentJointIndex].size(); ++i)
        {
            apply_interpolation_to_joints_SLOW(
                scene,
                pAnimationData,
                bindPose,
                current,
                next,
                amount,
                bindPose.jointChildMapping[currentJointIndex][i],
                m
            );
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
        ComponentPool& animDataPool = pScene->componentPools[ComponentType::PK_ANIMATION_DATA];

        uint64_t requiredMask = ComponentType::PK_ANIMATION_DATA | ComponentType::PK_TRANSFORM;
        for (Entity e : pScene->entities)
        {
            if ((e.componentMask & requiredMask) == requiredMask)
            {
                AnimationData* pAnimData = (AnimationData*)animDataPool[e.id];
                if (!pAnimData->isActive())
                    continue;

                // Override stopped if mode changed elsewhere
                if (pAnimData->_mode == AnimationMode::PK_ANIMATION_MODE_REPEAT)
                    pAnimData->_stopped = false;

                if (pAnimData->_stopped)
                    continue;

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
                size_t keyframeCount = pAnimationResource->getKeyframeCount();
                const AnimationMode& animMode = pAnimData->_mode;

                uint32_t& currentPoseIndex = pAnimData->_currentPose;
                uint32_t& nextPoseIndex = pAnimData->_nextPose;

                Pose currentPose;
                Pose nextPose;

                // TODO: Boundary checking!
                if (pAnimData->_keyframes.empty())
                {
                    currentPose = pAnimationResource->getPose(currentPoseIndex);
                    nextPose = pAnimationResource->getPose(nextPoseIndex);
                }
                else
                {
                    keyframeCount = pAnimData->_keyframes.size();
                    currentPose = pAnimationResource->getPose(pAnimData->_keyframes[currentPoseIndex]);
                    nextPose = pAnimationResource->getPose(pAnimData->_keyframes[nextPoseIndex]);
                }

                /*
                apply_interpolation_to_joints(
                    *pScene,
                    pAnimData,
                    pAnimationResource->getBindPose(),
                    currentPose,
                    nextPose,
                    pAnimData->_progress,
                    0,
                    mat4(1.0f)
                );
                */
                apply_interpolation_to_joints_FAST(
                    *pScene,
                    pAnimData,
                    pAnimationResource->getBindPose(),
                    currentPose,
                    nextPose,
                    pAnimData->_progress,
                    0,
                    mat4(1.0f)
                );

                pAnimData->_progress += pAnimData->getSpeed() * Timing::get_delta_time();
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
