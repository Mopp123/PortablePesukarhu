#include "CameraUtils.h"
#include "core/Application.h"


namespace pk
{
    Camera* create_camera(const vec3& position, float pitch, float yaw)
    {
        Application* app = Application::get();
        const Window* window = app->getWindow();

        const float windowWidth = (float)window->getWidth();
        const float windowHeight = (float)window->getHeight();

        const float aspectRatio = windowWidth / windowHeight;
        mat4 projMat_ortho = create_proj_mat_ortho(0, windowWidth, windowHeight, 0, 0.0f, 100.0f);
        mat4 projMat_pers = create_perspective_projection_matrix(aspectRatio, 1.3f, 0.1f, 100.0f);

        Scene* currentScene = app->accessCurrentScene();

        uint32_t camEntity = currentScene->createEntity();

        Camera* camComponent = new Camera(projMat_ortho, projMat_pers);
        Transform* camTransform = new Transform(position, { 1,1,1 }, pitch, yaw);

        currentScene->addComponent(camEntity, camTransform);
        currentScene->addComponent(camEntity, camComponent);

        // We need to reconfigure projection matrices on window resize!
        app->accessInputManager()->addWindowResizeEvent(new CameraWindowResizeEvent(*camComponent));

        Debug::log("Camera created");

        return camComponent;
    }


    void CameraWindowResizeEvent::func(int w, int h)
    {
        const float aspectRatio = (float)w / (float)h;

        _camRef.setProjMat2D(create_proj_mat_ortho(0, w, h, 0, 0.0f, 100.0f));
        _camRef.setProjMat3D(create_perspective_projection_matrix(aspectRatio, 1.3f, 0.1f, 500.0f));

        Debug::log("Cam projections reconfigured!");
    }
}
