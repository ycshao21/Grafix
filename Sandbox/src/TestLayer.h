#pragma once

#include "Grafix.h"

class TestLayer : public Grafix::Layer
{
public:
    TestLayer();
    virtual ~TestLayer();

    virtual void OnUpdate(float ts) override;
private:
    Grafix::Shared<Grafix::Shader> m_Shader = nullptr;
    Grafix::Shared<Grafix::Shader> m_SquareShader = nullptr;

    Grafix::Shared<Grafix::VertexArray> m_VertexArray = nullptr;
    Grafix::Shared<Grafix::VertexBuffer> m_VertexBuffer = nullptr;
    Grafix::Shared<Grafix::IndexBuffer> m_IndexBuffer = nullptr;

    Grafix::Shared<Grafix::VertexArray> m_SquareVA= nullptr;
};
