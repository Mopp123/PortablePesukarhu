
#include "InputField.h"
#include "../../../../core/Debug.h"

namespace pk
{
	namespace ui
	{


		void  InputField::InputFieldKeyEvent::func(InputKeyName key, int scancode, InputAction action, int mods)
		{
			
			if (_inputFieldRef.isActive())
			{
				std::string& txt = _inputFieldRef._inputFieldButton.accessText().accessRenderable()->accessStr();// <#M_DANGER> Not sure how badly this 'll end up fucking something...

				if (action != PK_INPUT_RELEASE)
				{
					// Erasing front text if backspace
					if (key == PK_INPUT_KEY_BACKSPACE)
					{
						Debug::log("DELETING");
						if (!txt.empty())
							txt.pop_back();
					}
					// Default submit behaviour..
					if (key == PK_INPUT_KEY_ENTER)
					{
						if (_inputFieldRef._onSubmitEvent)
							_inputFieldRef._onSubmitEvent->onSubmit(txt);

						if (_inputFieldRef._clearOnSubmit)
							_inputFieldRef.clear();

						_inputFieldRef.deactivate();
					}
				}
			}
		}
		
		void InputField::InputFieldCharInputEvent::func(unsigned int codepoint)
		{
			if (_inputFieldRef.isActive())
			{
				// <#M_DANGER>
				// just for testing atm.. quite dumb way to do it like this..
				unsigned char typedChar = (unsigned char)codepoint;
				std::string& txt = _inputFieldRef._inputFieldButton.accessText().accessRenderable()->accessStr();
				txt.push_back(typedChar);
			}
		}


#define PK_INPUTFIELD_DEFAULT_HEIGHT 16

		InputField::InputField(
			std::string infoTxt,
			const std::vector<Constraint>& constraints,
			int width,
			InputFieldOnSubmitEvent* onSubmitEvent,
			bool clearOnSubmit
		) :
			_inputFieldButton("", constraints, (float)width, PK_INPUTFIELD_DEFAULT_HEIGHT, nullptr, true, 0, 0),
			_infoText(infoTxt, constraints),
			_onSubmitEvent(onSubmitEvent),
			_clearOnSubmit(clearOnSubmit)
		{
			
			Application* app = Application::get();
			InputManager* inputManager = app->accessInputManager();

			inputManager->addKeyEvent(new InputFieldKeyEvent(*this));
			inputManager->addCharInputEvent(new InputFieldCharInputEvent(*this));

			// ..don't remember why these.. but if no these, this gets fucked...
			_infoText.accessTransform()->accessTransformationMatrix()[0 + 0 * 4] = width;
			_infoText.accessTransform()->accessTransformationMatrix()[1 + 1 * 4] = PK_INPUTFIELD_DEFAULT_HEIGHT;

			_infoText.accessConstraints()[0].value += width;
		}

		InputField::~InputField()
		{
			delete _onSubmitEvent;
		}

		std::string InputField::getContent() const
		{
			return _inputFieldButton.getText().getRenderable()->getStr();
		}

		void InputField::clear()
		{
			_inputFieldButton.accessText().accessRenderable()->accessStr().clear();
		}


		void InputField::activate()
		{
			//_isActive = true;
		}
		void InputField::deactivate()
		{
			_inputFieldButton._isSelected = false;
			_inputFieldButton.accessImage().setHighlighted(false);
			//_isActive = false;
		}
	}
}