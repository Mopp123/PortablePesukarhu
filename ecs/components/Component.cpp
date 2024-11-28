#include "Component.h"


namespace pk
{
    std::string component_type_to_string(ComponentType type)
    {
        switch (type)
        {
            case (PK_EMPTY) : return "empty";
            case (PK_RENDERABLE_GUI) : return "GUIRenderable";
            case (PK_RENDERABLE_TEXT) : return "TextRenderable";
            case (PK_RENDERABLE_SPRITE3D) : return "SpriteRenderable";
            case (PK_RENDERABLE_STATIC3D) : return "Static3DRenderable";
            case (PK_UIELEM_STATE) : return "UIElementState";
            case (PK_LIGHT_DIRECTIONAL) : return "Directionallight";
            case (PK_TRANSFORM) : return "Transform";
            case (PK_UI_CONSTRAINT) : return "UIConstraint";
            case (PK_CAMERA) : return "Camera";
            default: return "empty";
        }
    }


    Component::Component()
    {}

    Component::Component(ComponentType type) :
        _type(type)
    {
    }

    Component::Component(const Component& other) :
        _type(other._type)
    {
    }

    Component::~Component()
    {
    }
}
