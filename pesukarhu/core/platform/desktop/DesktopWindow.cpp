#include "DesktopWindow.h"
#include "pesukarhu/core/Debug.h"
#include <string>


#define DEFAULT__OPENGL_VERSION_MAJOR 4
#define DEFAULT__OPENGL_VERSION_MINOR 0


namespace pk
{
    namespace desktop
    {
        DesktopWindow::DesktopWindow(
            GraphicsAPI graphicsAPI,
            const std::string& title,
            int width,
            int height,
            int MSAASamples,
            bool fullscreen
        ) :
            Window(width, height)
        {
            _fullscreen = fullscreen;
            if (!glfwInit())
            {
                Debug::log(
                    "@DesktopWindow::DesktopWindow "
                    "glfwInit failed!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }

            if (graphicsAPI == GraphicsAPI::PK_GRAPHICS_API_OPENGL)
            {
                if(MSAASamples > 0) glfwWindowHint(GLFW_SAMPLES, MSAASamples);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, DEFAULT__OPENGL_VERSION_MAJOR);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, DEFAULT__OPENGL_VERSION_MINOR);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
            }
            else
            {
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            }

            // TODO: Maybe support resizable?
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

            _pGLFWwindow = glfwCreateWindow(
                _width,
                _height,
                title.c_str(),
                _fullscreen ? glfwGetPrimaryMonitor() : NULL,
                NULL
            );

            if (!_pGLFWwindow)
            {
                Debug::log(
                    "@DesktopWindow::DesktopWindow "
                    "glfwCreateWindow failed!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                glfwTerminate();
            }

            Debug::log(
                "@DesktopWindow::DesktopWindow "
                "GLFW window created successfully"
            );
        }

        DesktopWindow::~DesktopWindow()
        {
            glfwDestroyWindow(_pGLFWwindow);
            glfwTerminate();
        }

        bool DesktopWindow::isCloseRequested() const
        {
            return (bool)(glfwWindowShouldClose(_pGLFWwindow));
        }

        void DesktopWindow::resize(int w, int h)
        {
        }
    }
}
