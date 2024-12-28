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
            std::unordered_map<int, InputKeyName> _glfwToPKKeyMapping;
            std::unordered_map<int, InputMouseButtonName> _glfwToPKMouseButtonMapping;
            std::unordered_map<int, InputAction> _glfwToPKActionMapping;

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
