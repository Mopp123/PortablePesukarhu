

#include "../pk/core/platform/web/WebWindow.h"
#include "../pk/core/input/platform/web/WebInputManager.h"
#include "../pk/core/Application.h"
#include "../pk/graphics/platform/web/WebContext.h"

#include "../pk/graphics/platform/web/WebMasterRenderer.h"
#include "../pk/graphics/platform/web/WebGUIRenderer.h"

#include "../pk/ecs/components/renderable/GUIRenderable.h"

#include "../pk/core/Debug.h"

#include <vector>

using namespace pk;
using namespace web;


class TestScene : public Scene
{
public:

	TestScene() {}
	~TestScene() {}

	virtual void init()
	{
		GUIRenderable* r1 = new GUIRenderable;
		r1->pos = vec2(0.1f, 0);
		r1->scale = vec2(0.5f, 0.5f);

		GUIRenderable* r2 = new GUIRenderable;
		r2->pos = vec2(-1, 0);
		r2->scale = vec2(0.25f, 0.5f);


		components[ComponentType::PK_RENDERABLE_GUI].push_back(r1);
		components[ComponentType::PK_RENDERABLE_GUI].push_back(r2);

	}

	virtual void update()
	{
		
	}
};


int main(int argc, const char** argv)
{
	bool initSuccess = true;

	WebWindow window(800, 600);
	WebContext graphicsContext;
	WebInputManager inputManager;

	WebMasterRenderer masterRenderer;
	Renderer* pGuiRenderer = (Renderer*)(new WebGUIRenderer);

	Application application("Emscripten testing..", &window, &graphicsContext, &inputManager, &masterRenderer, { { ComponentType::PK_RENDERABLE_GUI, pGuiRenderer } });

	application.switchScene(new TestScene);

	application.run();

	// set "main loop"
	//emscripten_set_main_loop(update, 0, 0);

	return 0;
}