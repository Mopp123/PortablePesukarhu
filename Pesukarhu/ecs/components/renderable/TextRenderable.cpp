#include "TextRenderable.h"
#include "core/Application.h"


namespace pk
{

    TextRenderable::TextRenderable(const std::string& txt, PK_id font, bool bold) :
        UIRenderableComponent(ComponentType::PK_RENDERABLE_TEXT),
        _txt(txt),
        _bold(bold),
        color(1, 1, 1),
        fontID(font)
    {
    }

    TextRenderable::TextRenderable(const std::string& txt, PK_id font, vec3 color, bool bold) :
        UIRenderableComponent(ComponentType::PK_RENDERABLE_TEXT),
        _txt(txt),
        _bold(bold),
        color(color),
        fontID(font)
    {
    }

    TextRenderable::TextRenderable(const TextRenderable& other) :
        UIRenderableComponent(other._type),
        _txt(other._txt),
        _bold(other._bold),
        color(other.color),
        fontID(other.fontID)
    {
    }

    TextRenderable* TextRenderable::create(
        entityID_t target,
        const std::string& txt,
        PK_id fontID,
        vec3 color,
        bool bold
    )
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        TextRenderable* pRenderable = (TextRenderable*)pScene->componentPools[ComponentType::PK_RENDERABLE_TEXT].allocComponent(target);
        *pRenderable = TextRenderable(txt, fontID, color, bold);
        pScene->addComponent(target, pRenderable);
        return pRenderable;
    }
}
