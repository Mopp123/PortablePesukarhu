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
        usePlatform = PlatformName::PK_PLATFORM_LINUX;
        useGraphicsAPI = GraphicsAPI::PK_GRAPHICS_API_OPENGL;
    #elif PK_BUILD_WEB
        usePlatform = PlatformName::PK_PLATFORM_WEB;
        useGraphicsAPI = GraphicsAPI::PK_GRAPHICS_API_WEBGL;
    #endif

    Window* pWindow = Window::create(usePlatform, useGraphicsAPI, "Testing", 800, 600);
    Context* pGraphicsContext = Context::create(usePlatform, useGraphicsAPI, pWindow);
    InputManager* pInputManager = InputManager::create(usePlatform, pWindow);

    Application application(
        usePlatform,
        "Desktop Test",
        pWindow,
        pGraphicsContext,
        pInputManager
    );
    application.switchScene((Scene*)(new UITestScene));
    application.run();

    return 0;
}
