#pragma once

#include "ecs/components/Component.h"
#include <unordered_map>


namespace pk
{
    class UIRenderableComponent : public Component
    {
    protected:
        // NOTE: WARNING! LAYERS ARE FUCKED ATM!!
        static int s_totLayerValCount;
        static int s_currentSelectedLayer;

        int _layerVal = 0;

    public:
        UIRenderableComponent(ComponentType type);
        UIRenderableComponent(const UIRenderableComponent& other);
        virtual ~UIRenderableComponent();

        static int get_current_selected_layer();
        static void set_current_selected_layer(int val);

        inline int getLayerVal() const { return _layerVal; }
    };
}
