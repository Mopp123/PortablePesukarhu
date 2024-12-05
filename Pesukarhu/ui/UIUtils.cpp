#include "UIUtils.h"
#include "Pesukarhu/core/Application.h"
#include "Pesukarhu/core/Debug.h"


namespace pk
{
    namespace ui
    {
        std::string get_input_field_content(entityID_t inputFieldEntity)
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            const UIElemState* pState = (const UIElemState*)pScene->getComponent(
                inputFieldEntity,
                ComponentType::PK_UIELEM_STATE
            );
            if (!pState)
            {
                Debug::log(
                    "@get_input_field_content "
                    "No valid UIElemState found for entity: " + std::to_string(inputFieldEntity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return "";
            }
            return pState->content;
        }


        void set_input_field_content(
            const std::string& str,
            entityID_t inputFieldEntity,
            entityID_t txtRenderableEntity
        )
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            UIElemState* pUIElemState = (UIElemState*)pScene->getComponent(
                inputFieldEntity,
                ComponentType::PK_UIELEM_STATE
            );
            TextRenderable* pTextRenderable = (TextRenderable*)pScene->getComponent(
                txtRenderableEntity,
                ComponentType::PK_RENDERABLE_TEXT
            );
            if (!pUIElemState)
            {
                Debug::log(
                    "@set_input_field_content "
                    "No valid UIElemState found for input field root entity: " + std::to_string(inputFieldEntity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            if (!pTextRenderable)
            {
                Debug::log(
                    "@set_input_field_content "
                    "No valid TextRenderable found for input field's text content entity: " + std::to_string(txtRenderableEntity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            pUIElemState->content = str;
            pTextRenderable->accessStr() = str;
        }


        bool get_checkbox_status(entityID_t checkboxEntity)
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            const UIElemState* pState = (const UIElemState*)pScene->getComponent(
                checkboxEntity,
                ComponentType::PK_UIELEM_STATE
            );
            if (!pState)
            {
                Debug::log(
                    "@get_checkbox_status "
                    "No valid UIElemState found for entity: " + std::to_string(checkboxEntity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return false;
            }
            return pState->checked;
        }

        void set_checkbox_status(
            entityID_t checkboxEntity,
            entityID_t checkedStatusImgEntity,
            bool checked
        )
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            UIElemState* pCheckboxState = (UIElemState*)pScene->getComponent(
                checkboxEntity,
                ComponentType::PK_UIELEM_STATE
            );
            GUIRenderable* pCheckedStatusRenderable = (GUIRenderable*)pScene->getComponent(
                checkedStatusImgEntity,
                ComponentType::PK_RENDERABLE_GUI
            );
            if (!pCheckboxState)
            {
                Debug::log(
                    "@get_checkbox_status "
                    "No valid UIElemState found for checkbox entity: " + std::to_string(checkboxEntity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            if (!pCheckedStatusRenderable)
            {
                Debug::log(
                    "@get_checkbox_status "
                    "No valid GUIRenderable found for checkbox checked status img entity: " + std::to_string(checkedStatusImgEntity),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            pCheckboxState->checked = checked;
            pCheckedStatusRenderable->setActive(checked);
        }
    }
}
