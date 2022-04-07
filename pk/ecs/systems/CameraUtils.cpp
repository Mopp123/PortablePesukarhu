
#include "CameraUtils.h"

#include "../../core/Application.h"

namespace pk
{
	Camera* create_camera(const vec3& position)
	{
		Application* app = Application::get();
		const Window* window = app->getWindow();

		const float windowWidth = (float)window->getWidth();
		const float windowHeight = (float)window->getHeight();

		const float aspectRatio = windowWidth / windowHeight;
		mat4 projMat_ortho = create_proj_mat_ortho(0, windowWidth, windowHeight, 0, 0.0f, 100.0f);
		mat4 projMat_pers = create_perspective_projection_matrix(aspectRatio, 1.3f, 0.1f, 500.0f);

		Scene* currentScene = app->accessCurrentScene();

		uint32_t camEntity = currentScene->createEntity();

		Camera* camComponent = new Camera(projMat_ortho, projMat_pers);
		Transform* camTransform = new Transform(position, { 1,1,1 });

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

	CameraController::CameraController(Camera& toControl, Scene* scene)
	{
		Application* app = Application::get();
		
		// Find the camera's entity's transformation matrix
		_pToControl = (Transform*)scene->getComponent(toControl.getEntity(), ComponentType::PK_TRANSFORM);

		app->accessInputManager()->addKeyEvent(new CamControllerKeyEvent(*this));
		scene->addSystem(this);
	}

	void CameraController::update()
	{
		mat4& transformationMatrix = _pToControl->accessTransformationMatrix();

		if (_moveForward)
			transformationMatrix[2 + 3 * 4] -= _movingSpeed * Timing::get_delta_time();
		else if (_moveBackwards)
			transformationMatrix[2 + 3 * 4] += _movingSpeed * Timing::get_delta_time();

		if (_moveLeft)
			transformationMatrix[0 + 3 * 4] -= _movingSpeed * Timing::get_delta_time();
		else if (_moveRight)
			transformationMatrix[0 + 3 * 4] += _movingSpeed * Timing::get_delta_time();
	
		if (_moveUp)
			transformationMatrix[1 + 3 * 4] += _movingSpeed * Timing::get_delta_time();
		else if (_moveDown)
			transformationMatrix[1 + 3 * 4] -= _movingSpeed * Timing::get_delta_time();

	}


	void CameraController::CamControllerKeyEvent::func(InputKeyName key, int scancode, InputAction action, int mods)
	{
		_camControlRef._moveForward = (key == InputKeyName::PK_INPUT_KEY_W) && (action != InputAction::PK_INPUT_RELEASE);
		_camControlRef._moveBackwards = (key == InputKeyName::PK_INPUT_KEY_S) && (action != InputAction::PK_INPUT_RELEASE);

		_camControlRef._moveLeft = (key == InputKeyName::PK_INPUT_KEY_A) && (action != InputAction::PK_INPUT_RELEASE);
		_camControlRef._moveRight = (key == InputKeyName::PK_INPUT_KEY_D) && (action != InputAction::PK_INPUT_RELEASE);
	
		_camControlRef._moveUp = (key == InputKeyName::PK_INPUT_KEY_SPACE) && (action != InputAction::PK_INPUT_RELEASE);
		_camControlRef._moveDown = (key == InputKeyName::PK_INPUT_KEY_LCTRL) && (action != InputAction::PK_INPUT_RELEASE);
	}
}