
#include "GUIImage.h"
#include "../../../core/Application.h"


namespace pk
{
	namespace ui
	{


		GUIImage::GUIImage(
			const std::vector<Constraint>& constraints,
			float width, float height,
			vec3 originalColor, vec3 highlightColor
		) :
			UIElement(constraints),
			_originalColor(originalColor), _highlightColor(highlightColor)
		{
			Application* app = Application::get();
			Scene* currentScene = app->getCurrentScene();

			_id = currentScene->createEntity();

			_transform = new Transform({ 0,0 }, { width, height });
			_renderable = new GUIRenderable;

			currentScene->addComponent(_id, _transform);
			currentScene->addComponent(_id, _renderable);

			currentScene->addSystem(this);

			app->accessInputManager()->addCursorPosEvent(new GUIImageCursorPosEvent(*this));
		}

		GUIImage::GUIImage(const GUIImage& other) : 
			UIElement(other._constraints),
			_id(other._id)
		{
			_transform = other._transform;
			_renderable = other._renderable;
		}

		GUIImage::~GUIImage()
		{
		}

		void GUIImage::setHighlighted(bool arg)
		{
			if (_renderable)
			{
				vec3& currentColor = _renderable->color;
				currentColor = arg ? _highlightColor : _originalColor;
			}
		}


		void GUIImageCursorPosEvent::func(int x, int y)
		{
			const mat4& tMat = _guiRef.accessTransform()->getTransformationMatrix();
			const int& xPos = tMat[0 + 3 * 4];
			const int& yPos = tMat[1 + 3 * 4];
			
			const int& width =	tMat[0 + 0 * 4];
			const int& height = tMat[1 + 1 * 4];

			_guiRef._isMouseOver = (x >= xPos && x <= xPos + width) && (y <= yPos && y >= yPos - height);
		}

	}
}