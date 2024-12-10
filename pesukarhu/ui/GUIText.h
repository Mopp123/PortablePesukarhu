#pragma once

#include "GUIElement.h"
#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/ecs/components/Transform.h"
#include "pesukarhu/ecs/components/renderable/TextRenderable.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/resources/Font.h"


namespace pk
{
    namespace ui
    {
        class GUIText : public GUIElement
        {
        public:
            GUIText(
                const std::string& str,
                const Font& font,
                ConstraintProperties constraintProperties,
                vec3 color = vec3(1.0f, 1.0f, 1.0f),
                bool bold = false
            );
            GUIText(const GUIText& other) = delete;

            std::string getInternalStr() const;
            std::string getVisualStr() const;
            void setInternalStr(const std::string& str);
            void setVisualStr(const std::string& str);

            Transform* getTransform();
            ConstraintData* getConstraint();
            TextRenderable* getRenderable();

            void setStr(const std::string& str);

            void setActive(bool arg);

        private:
            std::string getStr(bool getInternal) const;
            void setStr(const std::string& str, bool setInternal);
        };
    }
}
