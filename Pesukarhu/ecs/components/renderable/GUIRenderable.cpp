#include "GUIRenderable.h"
#include "core/Application.h"


namespace pk
{
    GUIRenderable::GUIRenderable(
        vec4 textureCropping,
        Texture* pTexture
    ) :
        UIRenderableComponent(ComponentType::PK_RENDERABLE_GUI),
        color(1, 1, 1),
        pTexture(pTexture),
        textureCropping(textureCropping),
        borderColor(vec4(color, 1.0f)),
        borderThickness(0.0f)
    {}

    GUIRenderable::GUIRenderable(const GUIRenderable& other) :
        UIRenderableComponent(other),
        color(other.color),
        pTexture(other.pTexture),
        textureCropping(other.textureCropping),
        borderColor(other.borderColor),
        borderThickness(other.borderThickness)
    {
    }

    GUIRenderable* GUIRenderable::create(
        entityID_t target,
        Texture* pTexture,
        vec3 color,
        vec4 borderColor,
        float borderThickness,
        vec4 textureCropping
    )
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        GUIRenderable* pRenderable = (GUIRenderable*)pScene->componentPools[ComponentType::PK_RENDERABLE_GUI].allocComponent(target);
        *pRenderable = GUIRenderable(textureCropping, pTexture);
        pRenderable->color = color;
        pRenderable->borderColor = borderColor;
        pRenderable->borderThickness = borderThickness;
        pScene->addComponent(target, pRenderable);
        return pRenderable;
    }
}
