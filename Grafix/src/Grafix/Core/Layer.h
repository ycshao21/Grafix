#pragma once

#include "Grafix/Events/Event.h"

namespace Grafix
{
    class Layer
    {
    public:
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float ts) {}
        virtual void OnUIRender() {}
        virtual void OnEvent(Event& e) {}
    };
}
