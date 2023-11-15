#pragma once

#include "Grafix.h"

class SandboxLayer : public Grafix::Layer
{
public:
    SandboxLayer();
    virtual ~SandboxLayer();

    virtual void OnUpdate(float ts) override;
};
