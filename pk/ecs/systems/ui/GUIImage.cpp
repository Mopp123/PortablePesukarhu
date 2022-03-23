
#include "GUIImage.h"
#include "../../../core/Application.h"

namespace pk
{
	namespace ui
	{


		GUIImage::GUIImage(const std::vector<Constraint>& constraints, float width, float height) :
			UIElement(constraints)
		{
			Scene* currentScene = Application::get()->getCurrentScene();

			_id = currentScene->createEntity();

			_transform = new Transform({ 0,0 }, { width, height });
			_renderable = new GUIRenderable;

			currentScene->addComponent(_id, _transform);
			currentScene->addComponent(_id, _renderable);
		}

		GUIImage::~GUIImage()
		{
		}

	}
}