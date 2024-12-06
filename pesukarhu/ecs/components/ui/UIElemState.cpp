#include "UIElemState.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    std::unordered_map<entityID_t, int> UIElemState::s_pickedLayers = {};

    UIElemState::UIElemState() : Component(PK_UIELEM_STATE) {}
    UIElemState::~UIElemState() {}

    UIElemState* UIElemState::create(entityID_t target)
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        UIElemState* pElemState = (UIElemState*)pScene->componentPools[ComponentType::PK_UIELEM_STATE].allocComponent(target);
        *pElemState = UIElemState(); // not sure if fukup here?
        pScene->addComponent(target, pElemState);
        return pElemState;
    }
}
