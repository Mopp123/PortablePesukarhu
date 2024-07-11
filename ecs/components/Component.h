#pragma once

#include <cstdint>


namespace pk
{

    class Scene;

    enum ComponentType
    {
        PK_EMPTY = 0x0,
        PK_RENDERABLE_GUI = 0x1,
        PK_RENDERABLE_TEXT = 0x2,
        //PK_RENDERABLE_GUI_NEW, // Dont remember what this was doing here..
        PK_RENDERABLE_SPRITE3D = 0x4,
        PK_RENDERABLE_TERRAINTILE = 0x8,
        PK_RENDERABLE_STATIC3D = 0x10,

        PK_UIELEM_STATE = 0x20,

        PK_LIGHT_DIRECTIONAL = 0x40,

        PK_TRANSFORM = 0x80,

        PK_UI_CONSTRAINT = 0x100, // PROBLEM: currently relying on that entity can have multiple components of this type

        PK_CAMERA = 0x200
    };

    class Component
    {
    protected:
        friend class Scene;
        ComponentType _type = ComponentType::PK_EMPTY;
        bool _isActive = true;

    public:
        Component();
        Component(ComponentType type);
        Component(const Component& other);
        virtual ~Component();

        inline ComponentType getType() const { return _type; }
        inline bool isActive() const { return _isActive; }

        inline void setActive(bool arg) { _isActive = arg; }

        inline bool isNull() const { return _type == ComponentType::PK_EMPTY; }
    };
}
