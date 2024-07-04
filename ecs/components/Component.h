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

        PK_UI_CONSTRAINT = 0x100,

        PK_CAMERA = 0x200
    };

    class Component
    {
    protected:
        friend class Scene;

        uint32_t _id = 0;

        uint32_t _entity = 0;
        ComponentType _type;

        bool _isActive = true;

    public:
        Component(ComponentType type);
        Component(const Component& other);
        virtual ~Component();

        inline uint32_t getID() const { return _id; }

        inline uint32_t getEntity() const { return _entity; }
        inline ComponentType getType() const { return _type; }
        inline bool isActive() const { return _isActive; }

        inline void setActive(bool arg) { _isActive = arg; }
    };
}
