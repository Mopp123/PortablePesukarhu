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
        static int s_maxLayers;

        int _layer = 0;

    public:
        UIRenderableComponent(ComponentType type);
        UIRenderableComponent(const UIRenderableComponent& other);
        virtual ~UIRenderableComponent();

        inline int getLayer() const { return _layer; }
        void setLayer(int layer);

        static int get_current_selected_layer();
        static void set_current_selected_layer(int val);
        static int get_max_layers();
    };
}
