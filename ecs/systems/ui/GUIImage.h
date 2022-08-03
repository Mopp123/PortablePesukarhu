#pragma once

#include "UIElement.h"
#include "../../../core/input/InputEvent.h"
#include "../../components/renderable/GUIRenderable.h"


#define PK_DEFAULT_COLOR vec3(0.125f,0.125f,0.125f)
#define PK_DEFAULT_HIGHLIGHT_COLOR vec3(0.2125f,0.2f,0.2f)


namespace pk
{
	namespace ui
	{
		class GUIImageCursorPosEvent;
		class Button;

		class GUIImage : public UIElement
		{
		private:

			friend class GUIImageCursorPosEvent;
			friend class Button;

			uint32_t _id = 0;

			GUIRenderable* _renderable = nullptr;

			bool _isMouseOver = false;
			vec3 _originalColor;
			vec3 _highlightColor;


		public:

			// *Give width and height in pixels(even they are floats here...)
			GUIImage(
				const std::vector<Constraint>& constraints, 
				float width, float height, 
				vec3 originalColor = PK_DEFAULT_COLOR, vec3 highlightColor = PK_DEFAULT_HIGHLIGHT_COLOR
			);

			GUIImage(const GUIImage& other);
			~GUIImage();

			void setHighlighted(bool arg);

			inline GUIRenderable* accessRenderable() { return _renderable; }
			inline bool isMouseOver() const { return _isMouseOver; }
		};

		class GUIImageCursorPosEvent : public CursorPosEvent
		{
		private:

			GUIImage& _guiRef;

		public:

			GUIImageCursorPosEvent(GUIImage& guiRef) : _guiRef(guiRef) {}
			virtual void func(int x, int y);
		};
	}
}