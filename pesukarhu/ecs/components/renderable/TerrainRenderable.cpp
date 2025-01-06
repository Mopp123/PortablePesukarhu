#include "TerrainRenderable.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    TerrainRenderable::TerrainRenderable(
        PK_id terrainMeshID,
        PK_id terrainMaterialID
    ) :
        Component(ComponentType::PK_RENDERABLE_TERRAIN),
        terrainMeshID(terrainMeshID),
        terrainMaterialID(terrainMaterialID)
    {}

    TerrainRenderable* TerrainRenderable::create(
        entityID_t target,
        PK_id terrainMeshID,
        PK_id terrainMaterialID
    )
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        if ((pScene->getEntity(target).componentMask & ComponentType::PK_TRANSFORM) != ComponentType::PK_TRANSFORM)
        {
            Debug::log(
                "@TerrainRenderable::create "
                "Attempted to create terrain renderable component for entity: " + std::to_string(target) + " "
                "with no Transform component! This prevents rendering if transform not specified!",
                Debug::MessageType::PK_WARNING
            );
        }
        TerrainRenderable* pRenderable = (TerrainRenderable*)pScene->componentPools[ComponentType::PK_RENDERABLE_TERRAIN].allocComponent(target);
        *pRenderable = TerrainRenderable(
            terrainMeshID,
            terrainMaterialID
        );
        pScene->addComponent(target, pRenderable);
        return pRenderable;
    }
}
