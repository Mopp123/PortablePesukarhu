#include "CameraUtils.h"

#include "../../core/Application.h"


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


    RTSCamController::RTSCamController(Camera& toControl, Scene* scene)
    {
        _pivotPoint = vec3(0, 10.0f, 0);

        Application* app = Application::get();

        // Find the camera's entity's transformation matrix
        _pToControl = (Transform*)scene->getComponent(toControl.getEntity(), ComponentType::PK_TRANSFORM);

        app->accessInputManager()->addKeyEvent(new RTSCamControllerKeyEvent(*this));
        app->accessInputManager()->addMouseButtonEvent(new RTSCamControllerMouseButtonEvent(*this));
        app->accessInputManager()->addCursorPosEvent(new RTSCamControllerCursorPosEvent(*this));
        app->accessInputManager()->addScrollEvent(new RTSCamControllerScrollEvent(*this));

        scene->addSystem(this);
    }

    void RTSCamController::update()
    {

        mat4& transformationMatrix = _pToControl->accessTransformationMatrix();


        movePivotPoint(_moveFlags);


        float horizontalDist = std::cos(_pitch) * _distToPivotPoint;
        float verticalDist = std::sin(_pitch) * _distToPivotPoint;

        mat4 translationMat;
        translationMat.setIdentity();
        translationMat[0 + 3 * 4] = _pivotPoint.x + std::sin(_yaw) * horizontalDist;
        translationMat[1 + 3 * 4] = _pivotPoint.y + verticalDist;
        translationMat[2 + 3 * 4] = _pivotPoint.z + std::cos(_yaw) * horizontalDist;

        mat4 rotMatYaw;
        rotMatYaw.setIdentity();
        rotMatYaw[0 + 0 * 4] =	std::cos(_yaw);
        rotMatYaw[0 + 2 * 4] =	std::sin(_yaw);
        rotMatYaw[2 + 0 * 4] = -std::sin(_yaw);
        rotMatYaw[2 + 2 * 4] =	std::cos(_yaw);

        mat4 rotMatPitch;
        rotMatPitch.setIdentity();
        rotMatPitch[1 + 1 * 4] =  std::cos(-_pitch);
        rotMatPitch[1 + 2 * 4] = -std::sin(-_pitch);
        rotMatPitch[2 + 1 * 4] =  std::sin(-_pitch);
        rotMatPitch[2 + 2 * 4] =  std::cos(-_pitch);

        transformationMatrix = translationMat * rotMatYaw * rotMatPitch;
    }


    void RTSCamController::movePivotPoint(int moveFlags)
    {
        if (moveFlags == 0)
            return;

        float& xPos = _pivotPoint.x;
        float& zPos = _pivotPoint.z;

        float rotatedAngle = _yaw + M_PI * 0.5f;

        if (moveFlags & (int)MoveFlags::CAM_MOVE_DIR_FORWARD)
        {
            xPos += std::cos(rotatedAngle) * _movingSpeed * Timing::get_delta_time();
            zPos += -std::sin(rotatedAngle) * _movingSpeed * Timing::get_delta_time();
        }
        else if (moveFlags & (int)MoveFlags::CAM_MOVE_DIR_BACKWARDS)
        {
            xPos += std::cos(rotatedAngle) * -_movingSpeed * Timing::get_delta_time();
            zPos += -std::sin(rotatedAngle) * -_movingSpeed * Timing::get_delta_time();
        }

        if (moveFlags & (int)MoveFlags::CAM_MOVE_DIR_LEFT)
        {
            xPos += std::cos(_yaw) * -_movingSpeed * Timing::get_delta_time();
            zPos += -std::sin(_yaw) * -_movingSpeed * Timing::get_delta_time();
        }
        else if (moveFlags & (int)MoveFlags::CAM_MOVE_DIR_RIGHT)
        {
            xPos += std::cos(_yaw) * _movingSpeed * Timing::get_delta_time();
            zPos += -std::sin(_yaw) * _movingSpeed * Timing::get_delta_time();
        }
    }

    void RTSCamController::RTSCamControllerKeyEvent::func(InputKeyName key, int scancode, InputAction action, int mods)
    {
        if (action != InputAction::PK_INPUT_RELEASE)
        {


            if (key == InputKeyName::PK_INPUT_KEY_W)
                _camControlRef._moveFlags |= (int)RTSCamController::MoveFlags::CAM_MOVE_DIR_FORWARD;
            else if (key == InputKeyName::PK_INPUT_KEY_S)
                _camControlRef._moveFlags |= (int)RTSCamController::MoveFlags::CAM_MOVE_DIR_BACKWARDS;

            if (key == InputKeyName::PK_INPUT_KEY_D)
                _camControlRef._moveFlags |= (int)RTSCamController::MoveFlags::CAM_MOVE_DIR_RIGHT;
            else if (key == InputKeyName::PK_INPUT_KEY_A)
                _camControlRef._moveFlags |= (int)RTSCamController::MoveFlags::CAM_MOVE_DIR_LEFT;
        }
        else
        {
            if (key == InputKeyName::PK_INPUT_KEY_W)
                _camControlRef._moveFlags ^= (int)RTSCamController::MoveFlags::CAM_MOVE_DIR_FORWARD;
            if (key == InputKeyName::PK_INPUT_KEY_S)
                _camControlRef._moveFlags ^= (int)RTSCamController::MoveFlags::CAM_MOVE_DIR_BACKWARDS;

            if (key == InputKeyName::PK_INPUT_KEY_D)
                _camControlRef._moveFlags ^= (int)RTSCamController::MoveFlags::CAM_MOVE_DIR_RIGHT;
            if (key == InputKeyName::PK_INPUT_KEY_A)
                _camControlRef._moveFlags ^= (int)RTSCamController::MoveFlags::CAM_MOVE_DIR_LEFT;
        }
    }

    void RTSCamController::RTSCamControllerMouseButtonEvent::func(InputMouseButtonName button, InputAction action, int mods)
    {
        _camControlRef._enableRotating = button == InputMouseButtonName::PK_INPUT_MOUSE_MIDDLE && action != InputAction::PK_INPUT_RELEASE;
    }

    void RTSCamController::RTSCamControllerCursorPosEvent::func(int x, int y)
    {
        if (_camControlRef._enableRotating)
        {
            const float rotSpeedMod = 0.01f;

            float dx = (float)x - _camControlRef._prevMouseX;
            float dy = (float)y - _camControlRef._prevMouseY;

            _camControlRef._yaw -= dx * rotSpeedMod;
            _camControlRef._pitch -= dy * rotSpeedMod;
        }

        _camControlRef._prevMouseX = (float)x;
        _camControlRef._prevMouseY = (float)y;
    }

    void RTSCamController::RTSCamControllerScrollEvent::func(double dx, double dy)
    {
        _camControlRef._distToPivotPoint += (float)dy * _camControlRef._zoomingSpeed;
    }
}
