#pragma once

#include "../Component.h"
#include <unordered_map>

namespace pk
{
    class UIRenderableComponent : public Component
    {
    protected:
        static int s_totLayerValCount;
        static int s_currentSelectedLayer;

        int _layerVal = 0;

    public:
        UIRenderableComponent(ComponentType type);
        virtual ~UIRenderableComponent();

        static int get_current_selected_layer();
        static void set_current_selected_layer(int val);
        
        inline int getLayerVal() const { return _layerVal; }
    };
}
