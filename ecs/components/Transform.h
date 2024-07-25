#pragma once

#include "ecs//Entity.h"
#include "Component.h"
#include "../../utils/pkmath.h"

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

        void setPos(vec2 pos);
        void setPos(vec3 pos);

        void setRotation(float pitch, float yaw, float roll);
        void rotate(float pAmount, float yAmount, float rAmount);

        void setScale(vec2 scale);
        void setScale(vec3 scale);

        vec3 right() const;
        vec3 up() const;
        vec3 forward() const;

        inline vec2 getPos() const { return { _transformationMatrix[0 + 3 * 4], _transformationMatrix[1 + 3 * 4] }; }
        inline const mat4& getTransformationMatrix() const { return _transformationMatrix; }
        inline mat4& accessTransformationMatrix() { return _transformationMatrix; }
        inline mat4& accessLocalTransformationMatrix() { return _localTransformationMatrix; }

        inline bool hasParent() const { return _hasParent; }
    };
}
