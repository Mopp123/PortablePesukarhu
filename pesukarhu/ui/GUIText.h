#pragma once

#include "pesukarhu/ecs/Entity.h"
#include "pesukarhu/ecs/components/ui/ConstraintData.h"
#include "pesukarhu/ecs/components/Transform.h"
#include "pesukarhu/utils/pkmath.h"
#include "pesukarhu/resources/Font.h"


namespace pk
{
    namespace ui
    {
        class GUIText
        {
        private:
            entityID_t _entity = NULL_ENTITY_ID;

        public:
            GUIText() {}
            GUIText(const GUIText& other);
            void create(
                const std::string& str,
                const Font& font,
                ConstraintProperties constraintProperties,
                vec3 color = vec3(1.0f, 1.0f, 1.0f),
                bool bold = false
            );

            std::string getInternalStr() const;
            std::string getVisualStr() const;
            void setInternalStr(const std::string& str);
            void setVisualStr(const std::string& str);

            Transform* getTransform();

            void setActive(bool arg);

            inline entityID_t getEntity() const { return _entity; }

        private:
            std::string getStr(bool getInternal) const;
            void setStr(const std::string& str, bool setInternal);
        };
    }
}
