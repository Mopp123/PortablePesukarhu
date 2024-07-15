#pragma once

#include "UIRenderableComponent.h"
#include "../../../utils/pkmath.h"
#include "utils/ID.h"
#include <string>


namespace pk
{
    class TextRenderable : public UIRenderableComponent
    {
    private:
        std::string _txt;
        bool _bold;

    public:
        vec3 color;
        PK_id fontID = 0;

        TextRenderable(const std::string& txt, PK_id font, bool bold = false) :
            UIRenderableComponent(ComponentType::PK_RENDERABLE_TEXT),
            _txt(txt),
            _bold(bold),
            color(1, 1, 1),
            fontID(font)
        {
        }

        TextRenderable(const std::string& txt, PK_id font, vec3 color, bool bold = false) :
            UIRenderableComponent(ComponentType::PK_RENDERABLE_TEXT),
            _txt(txt),
            _bold(bold),
            color(color),
            fontID(font)
        {
        }

        TextRenderable(const TextRenderable& other) :
            UIRenderableComponent(other._type),
            _txt(other._txt),
            _bold(other._bold),
            color(other.color),
            fontID(other.fontID)
        {
        }

        inline std::string& accessStr() { return _txt; }
        inline void setColor(const vec3 color) { this->color = color; }
        inline const std::string& getStr() const { return _txt; }
        inline bool isBold() const { return _bold; }
        inline PK_id getFontID() const { return fontID; }
    };
}
