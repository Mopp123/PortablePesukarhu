#pragma once

#include "UIRenderableComponent.h"
#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/utils/ID.h"
#include <string>


namespace pk
{
    class TextRenderable : public UIRenderableComponent
    {
    private:
        std::string _txt;
        // Text to display. Used to have separate visual effect from the "actual content text"
        std::string _visualTxt;
        bool _bold;

    public:
        vec3 color;
        PK_id fontID = 0;

        TextRenderable(const std::string& txt, PK_id font, bool bold);
        TextRenderable(const std::string& txt, PK_id font, vec3 color, bool bold);
        TextRenderable(const TextRenderable& other);
        TextRenderable& operator=(TextRenderable&& other);

        static TextRenderable* create(
            entityID_t target,
            const std::string& txt,
            PK_id fontID,
            vec3 color,
            bool bold = false
        );

        std::string& accessStr();
        inline std::string& accessVisualStr() { return _visualTxt; }
        inline void setColor(const vec3 color) { this->color = color; }
        inline const std::string& getStr() const { return _txt; }
        inline const std::string& getVisualStr() const { return _visualTxt; }
        inline bool isBold() const { return _bold; }
        inline PK_id getFontID() const { return fontID; }
    };
}
