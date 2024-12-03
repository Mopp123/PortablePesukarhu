#include "Camera.h"
#include "core/Application.h"
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

    Camera* Camera::create(
        entityID_t target,
        const vec3& position,
        float pitch,
        float yaw
    )
    {
        Application* pApp = Application::get();
        const Window* window = pApp->getWindow();

        const float windowWidth = (float)window->getWidth();
        const float windowHeight = (float)window->getHeight();

        const float aspectRatio = windowWidth / windowHeight;
        const float farPlane2D = (float)(UIRenderableComponent::get_max_layers() + 1);
        const float nearPlane3D = 0.1f;
        mat4 orthographicProjMat = create_proj_mat_ortho(0, windowWidth, windowHeight, 0, 0.0f, farPlane2D);
        mat4 perspectivaProjMat = create_perspective_projection_matrix(aspectRatio, 1.3f, nearPlane3D, 1000.0f);

        Scene* pScene = pApp->accessCurrentScene();
        Camera* pCamera = (Camera*)pScene->componentPools[ComponentType::PK_CAMERA].allocComponent(target);
        *pCamera = Camera(orthographicProjMat, perspectivaProjMat, nearPlane3D);
        pScene->addComponent(target, pCamera);
        Transform::create(target, position, { 1, 1, 1 }, pitch, yaw);

        pApp->accessInputManager()->addWindowResizeEvent(new CameraWindowResizeEvent(*pCamera));
        return pCamera;
    }
}
