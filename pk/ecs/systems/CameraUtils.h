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

	class RTSCamController : public Updateable
	{
	private:

		class RTSCamControllerKeyEvent : public KeyEvent
		{
		private:
			RTSCamController& _camControlRef;
		public:
			RTSCamControllerKeyEvent(RTSCamController& camControl) : _camControlRef(camControl) {}
			virtual void func(InputKeyName key, int scancode, InputAction action, int mods);
		};
		class RTSCamControllerMouseButtonEvent : public MouseButtonEvent
		{
		private:
			RTSCamController& _camControlRef;
		public:
			RTSCamControllerMouseButtonEvent(RTSCamController& camControl) : _camControlRef(camControl) {}
			virtual void func(InputMouseButtonName button, InputAction action, int mods);
		};
		class RTSCamControllerCursorPosEvent : public CursorPosEvent
		{
		private:
			RTSCamController& _camControlRef;
		public:
			RTSCamControllerCursorPosEvent(RTSCamController& camControl) : _camControlRef(camControl) {}
			virtual void func(int x, int y);
		};
		class RTSCamControllerScrollEvent : public ScrollEvent
		{
		private:
			RTSCamController& _camControlRef;
		public:
			RTSCamControllerScrollEvent(RTSCamController& camControl) : _camControlRef(camControl) {}
			virtual void func(double dx, double dy);
		};

		friend class RTSCamControllerKeyEvent;
		friend class RTSCamControllerMouseButtonEvent;
		friend class RTSCamControllerCursorPosEvent;
		friend class RTSCamControllerScrollEvent;

		Transform* _pToControl = nullptr;

		float _movingSpeed = 20.0f;
		float _zoomingSpeed = 0.1f;

		enum MoveFlags
		{
			CAM_MOVE_DIR_FORWARD =		0x01,
			CAM_MOVE_DIR_BACKWARDS =	0x02,
			CAM_MOVE_DIR_LEFT =			0x04,
			CAM_MOVE_DIR_RIGHT =		0x08
		};

		int _moveFlags = 0;

		bool _enableRotating = false;

		vec3 _pivotPoint;
		float _distToPivotPoint = 20.0f;
		float _pitch = 0.0f;
		float _yaw = 0.0f;

		float _prevMouseX = 0.0f;
		float _prevMouseY = 0.0f;

	public:

		RTSCamController(Camera& toControl, Scene* scene);
		virtual void update();

		inline void setPivotPoint(const vec3& pos) { _pivotPoint = pos; }
		inline const vec3& getPivotPoint() const { return _pivotPoint; }
		
	private:

		void movePivotPoint(int moveFlags);
	};
}