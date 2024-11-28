#pragma once

#include <cstdint>
#include <string>


namespace pk
{

    class Scene;

    enum ComponentType
    {
        PK_EMPTY = 0x0,
        PK_RENDERABLE_GUI = 0x1,
        PK_RENDERABLE_TEXT = 0x2,
        PK_RENDERABLE_SPRITE3D = 0x4,
        PK_RENDERABLE_STATIC3D = 0x8,
        PK_RENDERABLE_SKINNED = 0x10,
        PK_RENDERABLE_TERRAIN = 0x20,

        PK_UIELEM_STATE = 0x40,

        PK_LIGHT_DIRECTIONAL = 0x80,

        PK_TRANSFORM = 0x100,

        PK_UI_CONSTRAINT = 0x200,

        PK_CAMERA = 0x400,

        PK_ANIMATION_DATA = 0x800,

        PK_BLINKER = 0x1000
    };


    std::string component_type_to_string(ComponentType);


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
