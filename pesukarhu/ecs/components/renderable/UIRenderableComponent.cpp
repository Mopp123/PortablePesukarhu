#include "UIRenderableComponent.h"
#include "pesukarhu/core/Debug.h"

namespace pk
{
    int UIRenderableComponent::s_totLayerValCount = 0;
    int UIRenderableComponent::s_currentSelectedLayer = 0;
    int UIRenderableComponent::s_maxLayers = 50;


    UIRenderableComponent::UIRenderableComponent(ComponentType type) :
       Component(type)
    {
        //_layerVal = s_totLayerValCount;
        //s_totLayerValCount++;
    }

    UIRenderableComponent::UIRenderableComponent(const UIRenderableComponent& other) :
        Component(other._type),
        _layer(other._layer)
    {

    }

    UIRenderableComponent::~UIRenderableComponent()
    {
        //s_totLayerValCount--;
        //if (s_totLayerValCount <= 0)
        //    s_currentSelectedLayer = 0;
    }

    void UIRenderableComponent::setLayer(int layer)
    {
        if (layer > s_maxLayers)
        {
            Debug::log(
                "@UIRenderableComponent::setLayer "
                "Layer out of bounds: " + std::to_string(layer) + " "
                "Max layer count is " + std::to_string(s_maxLayers),
                Debug::MessageType::PK_ERROR
            );
            return;
        }
        _layer = layer;
    }

    int UIRenderableComponent::get_current_selected_layer()
    {
        return s_currentSelectedLayer;
    }

    void UIRenderableComponent::set_current_selected_layer(int val)
    {
        s_currentSelectedLayer = val;
    }

    int UIRenderableComponent::get_max_layers()
    {
        return s_maxLayers;
    }
}
