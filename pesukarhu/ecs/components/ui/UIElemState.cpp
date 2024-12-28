#include "UIElemState.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    std::unordered_map<entityID_t, int> UIElemState::s_pickedLayers = {};

    UIElemState::UIElemState() :
        Component(PK_UIELEM_STATE)
    {}

    UIElemState::~UIElemState()
    {}

    // NOTE: IMPORTANT: Will be issues if base Component class changes!!!
    UIElemState& UIElemState::operator=(UIElemState&& other)
    {
        _type = other._type;
        mouseOver = other.mouseOver;
        selectable = other.selectable;
        selected = other.selected;
        state = other.state;
        clearOnSubmit = other.clearOnSubmit;
        checked = other.checked;
        _isActive = other._isActive;

        // Same string issue as in TextRenderable!
        //  -> see explanation in ecs/components/renderable/TextRenderable.cpp move operator
        content.resize(15);
        content = other.content;

        return *this;
    }

    UIElemState* UIElemState::create(entityID_t target)
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        UIElemState* pElemState = (UIElemState*)pScene->componentPools[ComponentType::PK_UIELEM_STATE].allocComponent(target);
        *pElemState = UIElemState(); // not sure if fukup here?
        pScene->addComponent(target, pElemState);
        return pElemState;
    }
}
