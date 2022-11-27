#pragma once

#include <cstdint>


namespace pk
{

    class Scene;

    enum ComponentType
    {
        PK_EMPTY,
        PK_RENDERABLE_GUI,
        PK_RENDERABLE_TEXT,
        PK_RENDERABLE_SPRITE3D,
        PK_RENDERABLE_TERRAINTILE,

        PK_UIELEM_STATE,

        PK_LIGHT_DIRECTIONAL,

        PK_TRANSFORM,

        PK_CAMERA

    };

    class Component
    {
    protected:
        friend class Scene;
    
        uint32_t _entity = 0;
        ComponentType _type;
    
        bool _isActive = true;

    public:
        Component(ComponentType type) : _type(type) { }
        virtual ~Component() { }
    
        inline uint32_t getEntity() const { return _entity; }
        inline ComponentType getType() const { return _type; }
        inline bool isActive() const { return _isActive; }
    
        inline void setActive(bool arg) { _isActive = arg; }
    };
}
