

#include "../pk/core/platform/web/WebWindow.h"
#include "../pk/core/input/platform/web/WebInputManager.h"
#include "../pk/core/Application.h"
#include "../pk/graphics/platform/web/WebContext.h"

#include "../pk/graphics/platform/web/WebMasterRenderer.h"

#include "../pk/core/Debug.h"


using namespace pk;
using namespace web;



int main(int argc, const char** argv)
{
	bool initSuccess = true;

	WebWindow window(800, 600);
	WebContext graphicsContext;
	WebInputManager inputManager;

	WebMasterRenderer masterRenderer;

	Application application("Emscripten testing..", &window, &masterRenderer);

	application.run();

	// set "main loop"
	//emscripten_set_main_loop(update, 0, 0);

	return 0;
}