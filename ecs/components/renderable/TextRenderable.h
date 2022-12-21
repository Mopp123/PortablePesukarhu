#pragma once

#include "UIRenderableComponent.h"
#include "../../../utils/pkmath.h"
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

        TextRenderable(const std::string& txt, bool bold = false) :
            UIRenderableComponent(ComponentType::PK_RENDERABLE_TEXT),
            _txt(txt),
            color(1, 1, 1),
            _bold(bold)
        {
        }

        TextRenderable(const std::string& txt, vec3 color, bool bold = false) :
            UIRenderableComponent(ComponentType::PK_RENDERABLE_TEXT),
            _txt(txt),
            color(color),
            _bold(bold)
        {
        }

        inline std::string& accessStr() { return _txt; }
        inline void setColor(const vec3 color) { this->color = color; }
        inline const std::string& getStr() const { return _txt; }
        inline bool isBold() const { return _bold; }
    };
}
