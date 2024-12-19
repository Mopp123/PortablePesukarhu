#pragma once

#include "pesukarhu/core/Window.h"
#include "pesukarhu/graphics/Context.h"
#include <GLFW/glfw3.h>


// EXPLANATION:
// This "desktop" namespace is used by both windows and linux sicne using glfw and glew
// the code is about identical...
namespace pk
{
    namespace desktop
    {
        class DesktopWindow : public Window
        {
        private:
            GLFWwindow* _pGLFWwindow = nullptr;

        public:
            DesktopWindow(
                GraphicsAPI graphicsAPI,
                const std::string& title,
                int width,
                int height,
                int MSAASamples,
                bool fullscreen
            );
            ~DesktopWindow();
            virtual bool isCloseRequested() const override;
            virtual void resize(int w, int h);

            void pollEvents_TEST();

            inline GLFWwindow* getGLFWwindow() { return _pGLFWwindow; }
        };
    }
}
