#include "UIUtils.h"
#include "Pesukarhu/core/Application.h"
#include "Pesukarhu/core/Debug.h"


namespace pk
{
    namespace ui
    {
        void set_input_field_content(
            const std::string& str,
            entityID_t inputFieldEntity,
            entityID_t txtRenderableEntity
        )
        {
            Scene* pScene = Application::get()->accessCurrentScene();
            if (!pScene)
            {
                Debug::log(
                    "@set_input_field_content "
                    "Current scene was nullptr!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            UIElemState* pUIElemState = (UIElemState*)pScene->getComponent(
                inputFieldEntity,
                ComponentType::PK_UIELEM_STATE
            );
            TextRenderable* pTextRenderable = (TextRenderable*)pScene->getComponent(
                txtRenderableEntity,
                ComponentType::PK_RENDERABLE_TEXT
            );
            if (pUIElemState)
                pUIElemState->content = str;
            if (pTextRenderable)
                pTextRenderable->accessStr() = str;
        }
    }
}
