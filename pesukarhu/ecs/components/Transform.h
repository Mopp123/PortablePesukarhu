#pragma once

#include "pesukarhu/ecs/Entity.h"
#include "Component.h"
#include "pesukarhu/utils/pkmath.h"


namespace pk
{
    class TransformSystem;;

    class Transform : public Component
    {
    private:
        friend class TransformSystem;

        mat4 _transformationMatrix;
        mat4 _localTransformationMatrix;

        bool _hasParent = false;

    public:
        Transform(vec2 pos, vec2 scale);
        Transform(vec3 pos, vec3 scale);
        Transform(vec3 pos, vec3 scale, float pitch, float yaw);
        Transform(vec3 pos, quat rotation, vec3 scale);
        Transform(mat4 matrix);
        Transform(const Transform& other);

        static Transform* create(entityID_t target, vec2 pos, vec2 scale);
        static Transform* create(entityID_t target, vec3 pos, vec3 scale);
        static Transform* create(entityID_t target, vec3 pos, vec3 scale, float pitch, float yaw);
        static Transform* create(entityID_t target, vec3 pos, quat rotation, vec3 scale);
        static Transform* create(entityID_t target, mat4 matrix);

        void setPos(vec2 pos);
        void setPos(vec3 pos);

        void setRotation(float pitch, float yaw, float roll);
        void setRotation(const quat& rotation);
        void rotate(float pAmount, float yAmount, float rAmount);

        void setScale(vec2 scale);
        void setScale(vec3 scale);

        vec3 right() const;
        vec3 up() const;
        vec3 forward() const;

        vec3 getLocalPos() const;
        vec3 getGlobalPos() const;
        vec3 getLocalScale() const;
        vec3 getGlobalScale() const;
        inline const mat4& getTransformationMatrix() const { return _transformationMatrix; }
        inline mat4& accessTransformationMatrix() { return _transformationMatrix; }
        inline mat4& accessLocalTransformationMatrix() { return _localTransformationMatrix; }

        inline bool hasParent() const { return _hasParent; }
    };
}
