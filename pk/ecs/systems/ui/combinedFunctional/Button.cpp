
#include "Button.h"
#include "../../../../core/Application.h"
#include "../../../../core/Debug.h"

namespace pk
{
	namespace ui
	{

		void Button::ButtonMouseButtonEvent::func(InputMouseButtonName button, InputAction action, int mods)
		{
			if (_buttonRef.getImage().isMouseOver())
			{
				if (action == PK_INPUT_PRESS)
					_buttonRef._state = 1;
				else if (_buttonRef._state == 1 && action == PK_INPUT_RELEASE)
					_buttonRef._state = 2;

				// state: 0 = "nothing"
				// state: 1 = pressed inside button
				// state: 2 = pressed and released inside button

				
				if (_buttonRef._state == 2)
				{
					if (_buttonRef._selectable)
					{
						_buttonRef._isSelected = !_buttonRef._isSelected;
						_buttonRef._img.setHighlighted(_buttonRef._isSelected);
					}

					if (_buttonRef._onClickEvent)
						_buttonRef._onClickEvent->onClick(button);

					_buttonRef._state = 0;
				}
			}
			else
			{
				if (_buttonRef._selectable)
				{
					_buttonRef._isSelected = false;
					_buttonRef._img.setHighlighted(false);
				}

				_buttonRef._state = 0;
			}
		}

		void Button::ButtonMouseCursorPosEvent::func(int x, int y)
		{
			GUIImage& imgRef = _buttonRef._img;
			
			if (!_buttonRef._isSelected)
				imgRef.setHighlighted(imgRef._isMouseOver);
			
		}



		Button::Button(
			std::string txt,
			const std::vector<Constraint>& constraints,
			float width, float height,
			OnClickEvent* onClick,
			bool selectable,
			int txtDisplacementX,
			int txtDisplacementY
		) :
			_img(constraints, width, height),
			_text(txt, constraints, false),
			_onClickEvent(onClick),
			_selectable(selectable)
		{
			
			_img.accessRenderable()->color = vec3(0.1f, 0.1f, 0.1f);

			applyTxtDisplacement(txtDisplacementX, txtDisplacementY);

			// ... not sure why i did this???
			_text.accessTransform()->accessTransformationMatrix()[0 + 0 * 4] = width;
			_text.accessTransform()->accessTransformationMatrix()[1 + 1 * 4] = height;

			//InputManager::get()->addMouseButtonEvent(new ButtonMouseButtonEvent(*this));

			Application::get()->accessInputManager()->addMouseButtonEvent(new ButtonMouseButtonEvent(*this));
			Application::get()->accessInputManager()->addCursorPosEvent(new ButtonMouseCursorPosEvent(*this));
		}


		Button::~Button()
		{
			delete _onClickEvent;
		}


		// Adds some displacement to the text's pos, so it could looks better.., -> so its not pixel perfectly aligned with the panel's corner
		// Quite fucking disgusting.. i kno..
		void Button::applyTxtDisplacement(int x, int y)
		{
			for (Constraint& c : _text.accessConstraints())
			{
				if (c.type == ConstraintType::PIXEL_TOP)
					c.value += (float)y;
				else if (c.type == ConstraintType::PIXEL_LEFT)
					c.value += (float)x;
				else if (c.type == ConstraintType::PIXEL_RIGHT)
					c.value -= (float)x;
				else if (c.type == ConstraintType::PIXEL_BOTTOM)
					c.value -= (float)y;
			}
		}



		/*
		void ButtonMouseButtonEvent::func(int button, int action, int mods)
		{
			if (!_buttonRef._canvasRef.isActive())
				return;

			if (_buttonRef._panel->isMouseOver())
			{
				if (button == 0)
				{
					if (action == GLFW_PRESS)
						_buttonRef._state = 1;
					else if (_buttonRef._state == 1 && action == GLFW_RELEASE)
						_buttonRef._state = 2;

				}
				// state: 0 = "nothing"
				// state: 1 = pressed inside button
				// state: 2 = pressed and released inside button

				if (_buttonRef._state == 2)
				{
					if (_buttonRef._onClickEvent)
						_buttonRef._onClickEvent->onClick(button, action);

					_buttonRef._state = 0;
				}
			}
			else
			{
				_buttonRef._state = 0;
			}

		}
		*/
	}
}