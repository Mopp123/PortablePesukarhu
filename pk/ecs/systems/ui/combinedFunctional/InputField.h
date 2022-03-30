#pragma once

#include "Button.h"


namespace pk
{
	namespace ui
	{

		class InputFieldOnSubmitEvent
		{
		public:

			virtual void onSubmit(std::string inputFieldText) = 0;
			virtual ~InputFieldOnSubmitEvent() {}
		};

		class InputField
		{
		private:


			class InputFieldKeyEvent : public KeyEvent
			{
			private:
				InputField& _inputFieldRef;
			public:
				InputFieldKeyEvent(InputField& inputField) : _inputFieldRef(inputField) {}
				virtual void func(InputKeyName key, int scancode, InputAction action, int mods);
			};

			class InputFieldCharInputEvent : public CharInputEvent
			{
			private:
				InputField& _inputFieldRef;
			public:
				InputFieldCharInputEvent(InputField& inputField) : _inputFieldRef(inputField) {}
				virtual void func(unsigned int codepoint);
			};


			Button _inputFieldButton; // This contains the "actual text content" of the input field
			Text _infoText;

			bool _clearOnSubmit = false;

			InputFieldOnSubmitEvent* _onSubmitEvent = nullptr;

		public:
			InputField(
				std::string infoTxt,
				const std::vector<Constraint>& constraints,
				int width,
				InputFieldOnSubmitEvent* onSubmitEvent,
				bool clearOnSubmit = false
			);

			~InputField();

			std::string getContent() const;

			virtual void onSubmit() {}

			void clear();

			void activate();
			void deactivate();

			bool isActive() const { return _inputFieldButton.isSelected(); }
		};


		

	}
}