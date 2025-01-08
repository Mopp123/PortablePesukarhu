#pragma once

#include "pesukarhu/ecs/Entity.h"


namespace pk
{
    namespace ui
    {
        enum GUIElementType
        {
            PK_GUI_ELEMENT_TYPE_NONE = 0,
            PK_GUI_ELEMENT_TYPE_IMAGE,
            PK_GUI_ELEMENT_TYPE_TEXT,
            PK_GUI_ELEMENT_TYPE_BUTTON,
            PK_GUI_ELEMENT_TYPE_INPUT_FIELD,
            PK_GUI_ELEMENT_TYPE_CHECKBOX,
            PK_GUI_ELEMENT_TYPE_SELECt
        };

        class GUIElement
        {
        protected:
            GUIElementType _type = GUIElementType::PK_GUI_ELEMENT_TYPE_NONE;
            entityID_t _entity = NULL_ENTITY_ID;

        public:
            GUIElement(GUIElementType type) : _type(type) {}
            GUIElement(const GUIElement& other) :
                _type(other._type),
                _entity(other._entity)
            {}
            virtual ~GUIElement() {}

            virtual void setActive(bool arg) = 0;
            virtual void setConstraintValues(float horizontal, float vertical) = 0;

            inline GUIElementType getType() const { return _type; }
            inline entityID_t getEntity() const { return _entity; }
        };
    }
}
