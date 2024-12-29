#pragma once

#include "pesukarhu/core/input/InputManager.h"
#include "pesukarhu/core/platform/desktop/DesktopWindow.h"
#include <unordered_map>
#include <string>


namespace pk
{
    namespace desktop
    {
        class DesktopInputManager : public InputManager
        {
        private:
            const std::unordered_map<int, InputKeyName> _glfwToPKKeyMapping
            {
                { GLFW_KEY_0, PK_INPUT_KEY_0 },
                { GLFW_KEY_1, PK_INPUT_KEY_1 },
                { GLFW_KEY_2, PK_INPUT_KEY_2 },
                { GLFW_KEY_3, PK_INPUT_KEY_3 },
                { GLFW_KEY_4, PK_INPUT_KEY_4 },
                { GLFW_KEY_5, PK_INPUT_KEY_5 },
                { GLFW_KEY_6, PK_INPUT_KEY_6 },
                { GLFW_KEY_7, PK_INPUT_KEY_7 },
                { GLFW_KEY_8, PK_INPUT_KEY_8 },
                { GLFW_KEY_9, PK_INPUT_KEY_9 },

                { GLFW_KEY_F1, PK_INPUT_KEY_F1 },
                { GLFW_KEY_F2, PK_INPUT_KEY_F2 },
                { GLFW_KEY_F3, PK_INPUT_KEY_F3 },
                { GLFW_KEY_F4, PK_INPUT_KEY_F4 },
                { GLFW_KEY_F5, PK_INPUT_KEY_F5 },
                { GLFW_KEY_F6, PK_INPUT_KEY_F6 },
                { GLFW_KEY_F7, PK_INPUT_KEY_F7 },
                { GLFW_KEY_F8, PK_INPUT_KEY_F8 },
                { GLFW_KEY_F9, PK_INPUT_KEY_F9 },
                { GLFW_KEY_F10, PK_INPUT_KEY_F10 },
                { GLFW_KEY_F11, PK_INPUT_KEY_F11 },
                { GLFW_KEY_F12, PK_INPUT_KEY_F12 },

                { GLFW_KEY_Q, PK_INPUT_KEY_Q },
                { GLFW_KEY_W, PK_INPUT_KEY_W },
                { GLFW_KEY_E, PK_INPUT_KEY_E },
                { GLFW_KEY_R, PK_INPUT_KEY_R },
                { GLFW_KEY_T, PK_INPUT_KEY_T },
                { GLFW_KEY_Y, PK_INPUT_KEY_Y },
                { GLFW_KEY_U, PK_INPUT_KEY_U },
                { GLFW_KEY_I, PK_INPUT_KEY_I },
                { GLFW_KEY_O, PK_INPUT_KEY_O },
                { GLFW_KEY_P, PK_INPUT_KEY_P },
                { GLFW_KEY_A, PK_INPUT_KEY_A },
                { GLFW_KEY_S, PK_INPUT_KEY_S },
                { GLFW_KEY_D, PK_INPUT_KEY_D },
                { GLFW_KEY_F, PK_INPUT_KEY_F },
                { GLFW_KEY_G, PK_INPUT_KEY_G },
                { GLFW_KEY_H, PK_INPUT_KEY_H },
                { GLFW_KEY_J, PK_INPUT_KEY_J },
                { GLFW_KEY_K, PK_INPUT_KEY_K },
                { GLFW_KEY_L, PK_INPUT_KEY_L },
                { GLFW_KEY_Z, PK_INPUT_KEY_Z },
                { GLFW_KEY_X, PK_INPUT_KEY_X },
                { GLFW_KEY_C, PK_INPUT_KEY_C },
                { GLFW_KEY_V, PK_INPUT_KEY_V },
                { GLFW_KEY_B, PK_INPUT_KEY_B },
                { GLFW_KEY_N, PK_INPUT_KEY_N },
                { GLFW_KEY_M, PK_INPUT_KEY_M },

                { GLFW_KEY_UP, PK_INPUT_KEY_UP },
                { GLFW_KEY_DOWN, PK_INPUT_KEY_DOWN },
                { GLFW_KEY_LEFT, PK_INPUT_KEY_LEFT },
                { GLFW_KEY_RIGHT, PK_INPUT_KEY_RIGHT},

                { GLFW_KEY_SPACE, PK_INPUT_KEY_SPACE },
                { GLFW_KEY_BACKSPACE, PK_INPUT_KEY_BACKSPACE },
                { GLFW_KEY_ENTER, PK_INPUT_KEY_ENTER },
                { GLFW_KEY_LEFT_CONTROL, PK_INPUT_KEY_LCTRL },
                { GLFW_KEY_LEFT_SHIFT, PK_INPUT_KEY_SHIFT },
                { GLFW_KEY_TAB, PK_INPUT_KEY_TAB },
                { GLFW_KEY_ESCAPE, PK_INPUT_KEY_ESCAPE}
            };

            const std::unordered_map<int, InputMouseButtonName> _glfwToPKMouseButtonMapping
            {
                { GLFW_MOUSE_BUTTON_LEFT, PK_INPUT_MOUSE_LEFT },
                { GLFW_MOUSE_BUTTON_RIGHT, PK_INPUT_MOUSE_RIGHT },
                { GLFW_MOUSE_BUTTON_MIDDLE, PK_INPUT_MOUSE_MIDDLE }
            };

            const std::unordered_map<int, InputAction> _glfwToPKActionMapping
            {
                { GLFW_RELEASE, PK_INPUT_RELEASE },
                { GLFW_PRESS, PK_INPUT_PRESS }
            };

        public:
            DesktopInputManager(DesktopWindow* pWindow);
            DesktopInputManager(const DesktopInputManager&) = delete;
            ~DesktopInputManager();

            virtual void pollEvents() override;

            InputKeyName getKeyName(int glfwKey) const;
            InputMouseButtonName getMouseButtonName(int glfwButton) const;
            InputAction getInputAction(int glfwAction) const;
        };
    }
}
