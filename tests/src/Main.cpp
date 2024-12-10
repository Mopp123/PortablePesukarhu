#include "pesukarhu/ppk.h"

#include "UITestScene.h"

using namespace pk;
using namespace pk::web;


int main(int argc, const char** argv)
{
    // NOTE: ISSUES!
    // * need to create window and input manager to heap using
    // some "create func" like the other api/platform agnostic stuff..

    // TODO: platform agnostic window creation
    WebWindow window;
    Context* pGraphicsContext = Context::create(GRAPHICS_API_WEBGL);
    WebInputManager inputManager;

    Application application(
        PK_PLATFORM_ID_WEB,
        "pesukarhuWebTest",
        &window,
        pGraphicsContext,
        &inputManager
    );
    application.switchScene((Scene*)(new UITestScene));
    application.run();

    return 0;
}
