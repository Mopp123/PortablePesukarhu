#pragma once

#include "../components/Camera.h"
#include "System.h"
#include "../../core/input/InputEvent.h"

namespace pk
{
	Camera* create_camera(const vec3& position);

	class CameraWindowResizeEvent : public WindowResizeEvent
	{
	private:
		Camera& _camRef;
	public:
		CameraWindowResizeEvent(Camera& cam) : _camRef(cam) {}
		virtual void func(int w, int h);
	};

	class CameraController : public Updateable
	{
	private:
		class CamControllerKeyEvent : public KeyEvent
		{
		private:
			
			CameraController& _camControlRef;

		public:

			CamControllerKeyEvent(CameraController& camControl) : _camControlRef(camControl) {}
			virtual void func(InputKeyName key, int scancode, InputAction action, int mods);
		};

		friend class CameraController;

		Transform* _pToControl = nullptr;

		float _movingSpeed = 10.0f;
		float _rotationSpeed = 5.0f;

		bool _moveForward = false;
		bool _moveBackwards = false;
		bool _moveLeft = false;
		bool _moveRight = false;

		bool _moveUp = false;
		bool _moveDown = false;

		bool _turnLeft = false;

		float angle = 0.0f;

	public:
		CameraController(Camera& toControl, Scene* scene);
		virtual void update();

	};
}