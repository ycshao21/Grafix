#pragma once

#include "Grafix.h"

class TestLayer : public Grafix::Layer
{
public:
    TestLayer();
    virtual ~TestLayer();

    virtual void OnUpdate(float ts) override;
    virtual void OnEvent(Grafix::Event& e) override;
private:
    // Shader
    Grafix::Shared<Grafix::Shader> m_SquareShader = nullptr;
    Grafix::Shared<Grafix::VertexArray> m_SquareVA= nullptr;

    Grafix::Shared<Grafix::Shader> m_Shader = nullptr;
    Grafix::Shared<Grafix::VertexArray> m_VertexArray = nullptr;

    // Camara
    Grafix::OrthographicCamera m_Camera;

    glm::vec3 m_CameraPosition = glm::vec3(0.0f);
    float m_CameraMoveSpeed = 1.0f;

    float m_CameraRotation = 0.0f;
    float m_CameraRotationSpeed = 35.0f;
};
