#include "pch.h"
#include "LayerStack.h"

namespace Grafix
{
    LayerStack::~LayerStack()
    {
        for (Layer* layer : m_Layers)
        {
            layer->OnDetach();
            delete layer;
        }
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.emplace_back(layer);
        layer->OnAttach();
    }
}

