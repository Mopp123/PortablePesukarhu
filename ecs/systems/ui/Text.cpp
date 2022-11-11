#include "Text.h"
#include "../../../core/Application.h"


namespace pk
{
	namespace ui
	{


		Text::Text(const std::string& str, const std::vector<Constraint>& constraints, bool bold) :
			UIElement(constraints)
		{
			Scene* currentScene = Application::get()->accessCurrentScene();

			_id = currentScene->createEntity();

			_transform = new Transform({ 0,0 }, { 1,1 });
			_renderable = new TextRenderable(str, bold);
			
			currentScene->addComponent(_id, _transform);
			currentScene->addComponent(_id, _renderable);

			currentScene->addSystem(this);
		}

		Text::Text(const Text& other) :
			UIElement(other._constraints),
			_id(other._id)
		{
			_transform = other._transform;
			_renderable = other._renderable;
		}


		Text::~Text()
		{
		}

	}
}
