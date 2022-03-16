

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
		components[ComponentType::PK_RENDERABLE_GUI].push_back(new GUIRenderable);
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