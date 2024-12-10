#include "Static3DRenderable.h"
#include "pesukarhu/core/Application.h"


namespace pk
{

    Static3DRenderable::Static3DRenderable(PK_id meshID) :
        Component(ComponentType::PK_RENDERABLE_STATIC3D),
        meshID(meshID)
    {}

    Static3DRenderable* Static3DRenderable::create(entityID_t target, PK_id meshID)
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        if ((pScene->getEntity(target).componentMask & ComponentType::PK_TRANSFORM) != ComponentType::PK_TRANSFORM)
        {
            Debug::log(
                "@Static3DRenderable::create "
                "Attempted to create renderable component for entity: " + std::to_string(target) + " "
                "with no Transform component! This prevents rendering if transform not specified!",
                Debug::MessageType::PK_WARNING
            );
        }

        Static3DRenderable* pRenderable = (Static3DRenderable*)pScene->componentPools[ComponentType::PK_RENDERABLE_STATIC3D].allocComponent(target);
        *pRenderable = Static3DRenderable(meshID);
        pScene->addComponent(target, pRenderable);
        return pRenderable;
    }
}
