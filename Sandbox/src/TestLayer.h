#pragma once

#include "Grafix.h"

class TestLayer : public Grafix::Layer
{
public:
    TestLayer();
    virtual ~TestLayer();

    virtual void OnUpdate(float ts) override;
};
