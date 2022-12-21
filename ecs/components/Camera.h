#pragma once

#include "Component.h"
#include "Transform.h"

#include "../../utils/pkmath.h"


namespace pk
{

    class Camera : public Component
    {
    private:
        mat4 _projMat2D;
        mat4 _projMat3D;

    public:
        Camera(const mat4& projMat2D, const mat4& projMat3D) :
            _projMat2D(projMat2D), _projMat3D(projMat3D),
            Component(ComponentType::PK_CAMERA)
        {}

        inline void setProjMat2D(const mat4& mat) { _projMat2D = mat; }
        inline void setProjMat3D(const mat4& mat) { _projMat3D = mat; }

        inline const mat4& getProjMat2D() const { return _projMat2D; }
        inline const mat4& getProjMat3D() const { return _projMat3D; }

    };
}
