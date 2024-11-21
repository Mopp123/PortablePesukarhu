#include "Camera.h"
#include "core/Debug.h"


namespace pk
{
    Camera::Camera(
        const mat4& projMat2D,
        const mat4& projMat3D,
        const float projMat3DNearPlane
    ) :
        Component(ComponentType::PK_CAMERA),
        _projMat2D(projMat2D),
        _projMat3D(projMat3D)
    {
        const float maxLayers = (const float)UIRenderableComponent::get_max_layers();
        _guiLayerMultiplier = projMat3DNearPlane / maxLayers;
    }

    Camera::Camera(const Camera& other) :
        Component(ComponentType::PK_CAMERA),
        _projMat2D(other._projMat2D),
        _projMat3D(other._projMat3D),
        _guiLayerMultiplier(other._guiLayerMultiplier)
    {
    }


    void CameraWindowResizeEvent::func(int w, int h)
    {
        const float aspectRatio = (float)w / (float)h;

        _camRef.setProjMat2D(create_proj_mat_ortho(0, w, h, 0, 0.0f, 100.0f));
        _camRef.setProjMat3D(create_perspective_projection_matrix(aspectRatio, 1.3f, 0.1f, 1000.0f));

        Debug::log("Cam projections reconfigured!");
    }
}
