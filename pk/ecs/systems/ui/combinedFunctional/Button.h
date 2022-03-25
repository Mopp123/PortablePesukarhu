#pragma once

#include "../../System.h"
#include "../GUIImage.h"
#include "../Text.h"


namespace pk
{
	namespace ui
	{

		//class OnClickEvent;

		class Button
		{
		protected:

			//friend class ButtonMouseButtonEvent;

			uint32_t _id;

			GUIImage _img;
			Text _text;

			//OnClickEvent* _onClickEvent = nullptr;

			int _state = 0;

		public:

			Button(
				std::string txt,
				const std::vector<Constraint>& constraints,
				float width, float height,
				int txtDisplacementX = 5,
				int txtDisplacementY = 5
			);

			Button(const Button& other) = delete;

			virtual ~Button();

			inline const GUIImage& getImage() { return _img; }
			inline const Text& getText() { return _text; }

			inline GUIImage& accessImage() { return _img; }
			inline Text& accessText() { return _text; }

		private:

			void applyTxtDisplacement(int x, int y);
		};

		/*
		class ButtonMouseButtonEvent : public MouseButtonEvent
		{
		private:
			Button& _buttonRef;
		public:
			ButtonMouseButtonEvent(Button& button) : _buttonRef(button) {}
			virtual void func(int button, int action, int mods) override;
		};


		class OnClickEvent
		{
		public:

			virtual void onClick(int button, int action) = 0;
		};*/
	}
}