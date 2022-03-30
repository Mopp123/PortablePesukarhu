#pragma once

#include "../../../../core/input/InputEvent.h"
#include "../../System.h"
#include "../GUIImage.h"
#include "../Text.h"



namespace pk
{
	namespace ui
	{

		class InputField;

		// * User defined OnClick
		class OnClickEvent
		{
		public:
			virtual void onClick(InputMouseButtonName button) = 0;
			virtual ~OnClickEvent(){}
		};


		class Button
		{
		protected:

			friend class InputField;

			uint32_t _id;

			GUIImage _img;
			Text _text;

			// Input event to trigger user defined OnClick
			class ButtonMouseButtonEvent : public MouseButtonEvent
			{
			private:
				Button& _buttonRef;
			public:
				ButtonMouseButtonEvent(Button& button) : _buttonRef(button) {}
				virtual void func(InputMouseButtonName button, InputAction action, int mods);
			};

			class ButtonMouseCursorPosEvent : public CursorPosEvent
			{
			private:
				Button& _buttonRef;
			public:
				ButtonMouseCursorPosEvent(Button& button) : _buttonRef(button) {}
				virtual void func(int x, int y);
			};

			OnClickEvent* _onClickEvent = nullptr;

			int _state = 0;

			bool _selectable = false;
			bool _isSelected = false;

		public:

			Button(
				std::string txt,
				const std::vector<Constraint>& constraints,
				float width, float height,
				OnClickEvent* onClick,
				bool selectable = false,
				int txtDisplacementX = 5,
				int txtDisplacementY = 5
			);

			Button(const Button& other) = delete;

			virtual ~Button();

			inline const GUIImage& getImage() { return _img; }
			inline const Text& getText() const { return _text; }

			inline GUIImage& accessImage() { return _img; }
			inline Text& accessText() { return _text; }
			
			inline bool isSelected() const { return _isSelected; }

		private:

			void applyTxtDisplacement(int x, int y);
		};
	}
}