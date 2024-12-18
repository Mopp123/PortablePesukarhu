#include "pesukarhu/ppk.h"
#include "UITestScene.h"

using namespace pk;


int main(int argc, const char** argv)
{
    PlatformName usePlatform = PlatformName::PK_PLATFORM_NONE;
    GraphicsAPI useGraphicsAPI = GraphicsAPI::PK_GRAPHICS_API_NONE;

    // NOTE: Atm just testing piece by piece to get Linux build working
    // TODO: Unify this shit after works properly!
    #ifdef PK_BUILD_LINUX
    Debug::log("___TEST___TESTING LINUX BUILD");

    #elif PK_BUILD_WEB
    usePlatform = PlatformName::PK_PLATFORM_WEB;
    useGraphicsAPI = GraphicsAPI::PK_GRAPHICS_API_WEBGL;

    Window* pWindow = Window::create(usePlatform, 800, 600);
    Context* pGraphicsContext = Context::create(useGraphicsAPI);
    InputManager* pInputManager = InputManager::create(usePlatform);

    Application application(
        PlatformName::PK_PLATFORM_WEB,
        "pesukarhuWebTest",
        pWindow,
        pGraphicsContext,
        pInputManager
    );
    application.switchScene((Scene*)(new UITestScene));
    application.run();
    #endif

    return 0;
}
