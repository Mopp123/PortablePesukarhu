#include "pesukarhu/ppk.h"
#include "UITestScene.h"

// testing TODO: delete
#include "pesukarhu/core/platform/desktop/DesktopWindow.h"

#include <iostream>

using namespace pk;


class KeyTest : public KeyEvent
{
public:
	virtual void func(InputKeyName key, int scancode, InputAction action, int mods)
    {
        if (key == InputKeyName::PK_INPUT_KEY_W && action == InputAction::PK_INPUT_PRESS)
            std::cout << "pressed w!\n";
        else if (key == InputKeyName::PK_INPUT_KEY_W && action == InputAction::PK_INPUT_RELEASE)
            std::cout << "released w!\n";
    }
};


int main(int argc, const char** argv)
{
    PlatformName usePlatform = PlatformName::PK_PLATFORM_NONE;
    GraphicsAPI useGraphicsAPI = GraphicsAPI::PK_GRAPHICS_API_NONE;

    // NOTE: Atm just testing piece by piece to get Linux build working
    // TODO: Unify this shit after works properly!
    #ifdef PK_BUILD_LINUX
    usePlatform = PlatformName::PK_PLATFORM_LINUX;
    useGraphicsAPI = GraphicsAPI::PK_GRAPHICS_API_OPENGL;

    Window* pWindow = Window::create(usePlatform, useGraphicsAPI, "Testing", 800, 600);
    desktop::DesktopWindow* pDesktopWindow = (desktop::DesktopWindow*)pWindow;
    Context* pGraphicsContext = Context::create(usePlatform, useGraphicsAPI, pWindow);
    InputManager* pInputManager = InputManager::create(usePlatform, pWindow);

    pInputManager->addKeyEvent(new KeyTest);

    while (!pWindow->isCloseRequested())
    {
        pDesktopWindow->pollEvents_TEST();
    }


    #elif PK_BUILD_WEB
    usePlatform = PlatformName::PK_PLATFORM_WEB;
    useGraphicsAPI = GraphicsAPI::PK_GRAPHICS_API_WEBGL;

    Window* pWindow = Window::create(usePlatform, "Testing", 800, 600);
    Context* pGraphicsContext = Context::create(usePlatform, useGraphicsAPI, pWindow);
    InputManager* pInputManager = InputManager::create(usePlatform, pWindow);

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
