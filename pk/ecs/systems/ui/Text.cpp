
#include "Text.h"
#include "../../../core/Application.h"

namespace pk
{
	namespace ui
	{


		Text::Text(const std::string& str, const std::vector<Constraint>& constraints, float scaleMod) :
			UIElement(constraints),
			_scaleModifier(scaleMod)
		{
			Scene* currentScene = Application::get()->getCurrentScene();

			_id = currentScene->createEntity();

			_transform = new Transform({ 0,0 }, { 1,1 });
			_renderable = new TextRenderable(str);
			
			currentScene->addComponent(_id, _transform);
			currentScene->addComponent(_id, _renderable);
		}

		Text::~Text()
		{
		}

	}
}