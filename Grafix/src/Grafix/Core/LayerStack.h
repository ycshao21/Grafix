#pragma once

#include "Layer.h"

#include <vector>

namespace Grafix
{
    class LayerStack final
    {
    public:
        ~LayerStack();

        void PushLayer(Layer* layer);

        // Iterators
        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
    private:
        std::vector<Layer*> m_Layers;
    };

}
