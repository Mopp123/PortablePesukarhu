#include "UIRenderableComponent.h"
#include "../../../core/Debug.h"

namespace pk
{
    int UIRenderableComponent::s_totLayerValCount = 0;
    int UIRenderableComponent::s_currentSelectedLayer = 0;

    
    UIRenderableComponent::UIRenderableComponent(ComponentType type) : 
       Component(type) 
    {
        _layerVal = s_totLayerValCount;
        s_totLayerValCount++;
    }
    
    UIRenderableComponent::~UIRenderableComponent() 
    {
        s_totLayerValCount--;
        if (s_totLayerValCount <= 0)
            s_currentSelectedLayer = 0;
    }

    int UIRenderableComponent::get_current_selected_layer()
    {
        return s_currentSelectedLayer;
    }
    void UIRenderableComponent::set_current_selected_layer(int val)
    {
        s_currentSelectedLayer = val;
    }
}
