#include "DesktopInputManager.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/Debug.h"
#include <GLFW/glfw3.h>


namespace pk
{
    namespace desktop
    {
        // NOTE: All below funcs are pretty new so there might be some keys/buttons wrong, etc..
        void key_callback(GLFWwindow* pGLFWwindow, int key, int scancode, int action, int mods)
        {
            DesktopInputManager* pInputManager = (DesktopInputManager*)glfwGetWindowUserPointer(pGLFWwindow);
            InputKeyName keyName = pInputManager->getKeyName(key);
            InputAction inputAction = pInputManager->getInputAction(action);
            pInputManager->keyDown[keyName] = action == GLFW_PRESS;
            pInputManager->processKeyEvents(keyName, scancode, inputAction, mods);
        }

        void char_callback(GLFWwindow* pGLFWwindow, unsigned int codepoint)
        {
            Debug::log("PROCESS CHARACTER EVENT: NOT TESTED!", Debug::MessageType::PK_WARNING);
            DesktopInputManager* pInputManager = (DesktopInputManager*)glfwGetWindowUserPointer(pGLFWwindow);
            pInputManager->processCharInputEvents(codepoint);
        }

        void mouse_button_callback(GLFWwindow* pGLFWwindow, int button, int action, int mods)
        {
            Debug::log("PROCESS MOUSE EVENT: BUTTON" + std::to_string(button) + " ACTION = " + std::to_string(action), Debug::MessageType::PK_WARNING);
            DesktopInputManager* pInputManager = (DesktopInputManager*)glfwGetWindowUserPointer(pGLFWwindow);
            InputMouseButtonName buttonName = pInputManager->getMouseButtonName(button);
            InputAction inputAction = pInputManager->getInputAction(action);
            pInputManager->mouseDown[buttonName] = action == GLFW_PRESS;
            pInputManager->processMouseButtonEvents(buttonName, inputAction, mods);
        }

        void cursor_pos_callback(GLFWwindow* pGLFWwindow, double x, double y)
        {
            DesktopInputManager* pInputManager = (DesktopInputManager*)glfwGetWindowUserPointer(pGLFWwindow);

            // In web gl our coords are flipped -> need to flip mouseY
            // NOTE: Not sure should we also with opengl!!
            const int windowWidth = Application::get()->getWindow()->getHeight();

            int mx = (int)x;
            int my = windowWidth - (int)y;
            pInputManager->setMousePos(mx, my);
            pInputManager->processCursorPosEvents(mx, my);
        }

        void scroll_callback(GLFWwindow* pGLFWwindow, double xOffset, double yOffset)
        {
            Debug::log("PROCESS SCROLL EVENT: NOT TESTED!", Debug::MessageType::PK_WARNING);
            DesktopInputManager* pInputManager = (DesktopInputManager*)glfwGetWindowUserPointer(pGLFWwindow);
            pInputManager->processScrollEvents(xOffset, yOffset);
        }

        void framebuffer_resize_callback(GLFWwindow* pGLFWwindow, int width, int height)
        {
            Debug::log("PROCESS WINDOW RESIZE EVENT: NOT TESTED!", Debug::MessageType::PK_WARNING);
            DesktopInputManager* pInputManager = (DesktopInputManager*)glfwGetWindowUserPointer(pGLFWwindow);
            Application::get()->resizeWindow(width, height);
            pInputManager->processWindowResizeEvents(width, height);
        }

        DesktopInputManager::DesktopInputManager(DesktopWindow* pWindow)
        {
            GLFWwindow* pGLFWwindow = pWindow->getGLFWwindow();
            glfwSetWindowUserPointer(pGLFWwindow, this);

            glfwSetKeyCallback(pGLFWwindow, key_callback);
            glfwSetCharCallback(pGLFWwindow, char_callback);
            glfwSetMouseButtonCallback(pGLFWwindow, mouse_button_callback);
            glfwSetCursorPosCallback(pGLFWwindow, cursor_pos_callback);
            glfwSetScrollCallback(pGLFWwindow, scroll_callback);

            glfwSetFramebufferSizeCallback(pGLFWwindow, framebuffer_resize_callback);
        }

        DesktopInputManager::~DesktopInputManager()
        {
        }

        void DesktopInputManager::pollEvents()
        {
            glfwPollEvents();
        }

        InputKeyName DesktopInputManager::getKeyName(int glfwKey) const
        {
            std::unordered_map<int, InputKeyName>::const_iterator it = _glfwToPKKeyMapping.find(glfwKey);
            if (it != _glfwToPKKeyMapping.end())
                return it->second;
            return InputKeyName::PK_INPUT_KEY_NOT_FOUND;
        }

        InputMouseButtonName DesktopInputManager::getMouseButtonName(int glfwButton) const
        {
            std::unordered_map<int, InputMouseButtonName>::const_iterator it = _glfwToPKMouseButtonMapping.find(glfwButton);
            if (it != _glfwToPKMouseButtonMapping.end())
                return it->second;
            return InputMouseButtonName::PK_INPUT_MOUSE_NOT_FOUND;
        }

        InputAction DesktopInputManager::getInputAction(int glfwAction) const
        {
            std::unordered_map<int, InputAction>::const_iterator it = _glfwToPKActionMapping.find(glfwAction);
            if (it != _glfwToPKActionMapping.end())
                return it->second;
            return InputAction::PK_INPUT_ACTION_NOT_FOUND;
        }
    }
}
