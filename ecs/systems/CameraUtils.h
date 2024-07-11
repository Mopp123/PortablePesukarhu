#pragma once

#include "../components/Camera.h"
#include "System.h"
#include "../../core/input/InputEvent.h"


namespace pk
{
    // TODO: delete below
    //Camera* create_camera(const vec3& position, float pitch = 0.0f, float yaw = 0.0f);

    class CameraWindowResizeEvent : public WindowResizeEvent
    {
    private:
        Camera& _camRef;
    public:
        CameraWindowResizeEvent(Camera& cam) : _camRef(cam) {}
        virtual void func(int w, int h);
    };
}
