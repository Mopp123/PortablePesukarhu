#pragma once

#include "Component.h"
#include "Transform.h"
#include "utils/pkmath.h"
#include "core/input/InputEvent.h"
#include "ecs/components/renderable/UIRenderableComponent.h"


namespace pk
{
    class Camera : public Component
    {
    private:
        mat4 _projMat2D;
        mat4 _projMat3D;

        // Explanation:
        // GUI rendering works atm in following way.
        // All gui gets rendered from the orthographic proj matrix's near plane at 0
        // until perspective proj matrix's near plane so the gui gets drawn always on top of
        // 3d stuff... we use this value in gui and text rendering to achieve this. ...fucking dumb, I know...
        float _guiLayerMultiplier = 0.0f;

    public:
        // projMat3DNearPlane is used to determine the "guiLayerMultiplier"
        Camera(
            const mat4& projMat2D,
            const mat4& projMat3D,
            const float projMat3DNearPlane
        );
        Camera(const Camera& other);

        inline void setProjMat2D(const mat4& mat) { _projMat2D = mat; }
        inline void setProjMat3D(const mat4& mat) { _projMat3D = mat; }

        inline const mat4& getProjMat2D() const { return _projMat2D; }
        inline const mat4& getProjMat3D() const { return _projMat3D; }
        inline float getGUILayerMultiplier() const { return _guiLayerMultiplier; }
    };


    class CameraWindowResizeEvent : public WindowResizeEvent
    {
    private:
        Camera& _camRef;
    public:
        CameraWindowResizeEvent(Camera& cam) : _camRef(cam) {}
        virtual void func(int w, int h);
    };
}
