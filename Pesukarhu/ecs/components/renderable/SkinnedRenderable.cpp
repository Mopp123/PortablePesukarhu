#include "SkinnedRenderable.h"
#include "Pesukarhu/core/Application.h"


namespace pk
{
    SkinnedRenderable::SkinnedRenderable(PK_id modelID, PK_id meshID) :
        Component(ComponentType::PK_RENDERABLE_SKINNED),
        modelID(modelID),
        meshID(meshID)
    {}


    SkinnedRenderable* SkinnedRenderable::create(
        entityID_t target,
        PK_id modelID,
        PK_id meshID
    )
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        if ((pScene->getEntity(target).componentMask & ComponentType::PK_TRANSFORM) != ComponentType::PK_TRANSFORM)
        {
            Debug::log(
                "@SkinnedRenderable::create "
                "Attempted to create renderable component for entity: " + std::to_string(target) + " "
                "with no Transform component! This prevents rendering if transform not specified!",
                Debug::MessageType::PK_WARNING
            );
        }

        SkinnedRenderable* pRenderable = (SkinnedRenderable*)pScene->componentPools[ComponentType::PK_RENDERABLE_SKINNED].allocComponent(target);
        *pRenderable = SkinnedRenderable(modelID, meshID);
        pScene->addComponent(target, pRenderable);
        return pRenderable;
    }
}
