#include "GUIRenderable.h"
#include "Pesukarhu/core/Application.h"


namespace pk
{
    GUIRenderable::GUIRenderable(
        vec4 textureCropping,
        Texture* pTexture
    ) :
        UIRenderableComponent(ComponentType::PK_RENDERABLE_GUI),
        color(1, 1, 1),
        pTexture(pTexture),
        textureCropping(textureCropping)
    {}

    GUIRenderable::GUIRenderable(const GUIRenderable& other) :
        UIRenderableComponent(other),
        color(other.color),
        pTexture(other.pTexture),
        textureCropping(other.textureCropping),
        filter(other.filter)
    {
    }

    GUIRenderable* GUIRenderable::create(
        entityID_t target,
        Texture* pTexture,
        vec3 color,
        GUIFilterType filter,
        vec4 textureCropping
    )
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        GUIRenderable* pRenderable = (GUIRenderable*)pScene->componentPools[ComponentType::PK_RENDERABLE_GUI].allocComponent(target);
        *pRenderable = GUIRenderable(textureCropping, pTexture);
        pRenderable->color = color;
        pRenderable->filter = filter;
        pScene->addComponent(target, pRenderable);
        return pRenderable;
    }
}
