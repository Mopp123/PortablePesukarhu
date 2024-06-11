#include "Component.h"
#include "core/Application.h"
#include "core/Scene.h"

namespace pk
{

    Component::Component(ComponentType type) :
        _type(type)
    {
        Scene* pCurrentScene = Application::get()->accessCurrentScene();
        _id = pCurrentScene->components.size() + 1;
        pCurrentScene->components[_id] = this;
        pCurrentScene->typeComponentMapping[_type].push_back(_id);
    }
}
