#include "TestLayer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Platform/OpenGL/OpenGLShader.h"

TestLayer::TestLayer()
    : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
{
    {
        m_VertexArray = Grafix::VertexArray::Create();

        // Vertex Buffer
        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.85f, 0.0f,  0.85f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.0f,  0.85f, 0.85f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.85f, 0.85f, 0.0f,  1.0f
        };
        auto vertexBuffer = Grafix::VertexBuffer::Create(vertices, sizeof(vertices));

        Grafix::VertexBufferLayout layout = {
            { Grafix::ShaderDataType::Float3, "a_Position" },
            { Grafix::ShaderDataType::Float4, "a_Color" }
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        // Index Buffer
        uint32_t indices[3] = { 0, 1, 2 };
        auto indexBuffer = Grafix::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        // Shader
        m_Shader = m_ShaderLibrary.LoadShader("assets/shaders/Colorful.glsl");
    }

    {
        m_SquareVA = Grafix::VertexArray::Create();

        // Vertex Buffer
        float vertices[4 * 3] = {
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f
        };
        auto squareVB = Grafix::VertexBuffer::Create(vertices, sizeof(vertices));

        squareVB->SetLayout({
            { Grafix::ShaderDataType::Float3, "a_Position" }
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
        auto squareIB = Grafix::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        m_SquareVA->SetIndexBuffer(squareIB);
        
        m_ShaderLibrary.LoadShader("assets/shaders/FlatColor.glsl");
        m_SquareShader = m_ShaderLibrary.GetShader("FlatColor");
    }
}

TestLayer::~TestLayer()
{
}

void TestLayer::OnUpdate(float ts)
{
    // Camera movement
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(m_CameraRotation), glm::vec3(0, 0, 1));
    if (Grafix::Input::IsKeyPressed(Grafix::Key::A))
    {
        m_CameraPosition -= m_CameraMoveSpeed * glm::vec3(rotate[0].x, rotate[0].y, rotate[0].z) * ts;
    }
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::D))
    {
        m_CameraPosition += m_CameraMoveSpeed * glm::vec3(rotate[0].x, rotate[0].y, rotate[0].z) * ts;
    }

    if (Grafix::Input::IsKeyPressed(Grafix::Key::W))
    {
        m_CameraPosition += m_CameraMoveSpeed * glm::vec3(rotate[1].x, rotate[1].y, rotate[1].z) * ts;
    }
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::S))
    {
        m_CameraPosition -= m_CameraMoveSpeed * glm::vec3(rotate[1].x, rotate[1].y, rotate[1].z) * ts;
    }

    // Camera rotation
    if (Grafix::Input::IsKeyPressed(Grafix::Key::Left))
    {
        m_CameraRotation += m_CameraRotationSpeed * ts;
    }
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::Right))
    {
        m_CameraRotation -= m_CameraRotationSpeed * ts;
    }

    // Triangle movement
    if (Grafix::Input::IsKeyPressed(Grafix::Key::J))
    {
        m_TrianglePosition.x -= m_TriangleMoveSpeed * ts;
    }
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::L))
    {
        m_TrianglePosition.x += m_TriangleMoveSpeed * ts;
    }

    if (Grafix::Input::IsKeyPressed(Grafix::Key::I))
    {
        m_TrianglePosition.y += m_TriangleMoveSpeed * ts;
    }
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::K))
    {
        m_TrianglePosition.y -= m_TriangleMoveSpeed * ts;
    }

    m_TriangleRotation += m_TriangleRotationSpeed * ts;


    // Reset camera
    if(Grafix::Input::IsKeyPressed(Grafix::Key::R))
    {
        m_CameraPosition = glm::vec3(0.0f);
        m_CameraRotation = 0.0f;
    }

    m_Camera.SetPosition(m_CameraPosition);
    m_Camera.SetRotation(m_CameraRotation);

    Grafix::Renderer::BeginScene(m_Camera);

    std::dynamic_pointer_cast<Grafix::OpenGLShader>(m_SquareShader)->Bind();
    std::dynamic_pointer_cast<Grafix::OpenGLShader>(m_SquareShader)->UploadUniformFloat4("u_Color", m_SquareColor);

    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_TrianglePosition) 
            * glm::rotate(glm::mat4(1.0f), glm::radians(m_TriangleRotation), glm::vec3(0.0f, 0.0f, 1.0f))
            * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
        Grafix::Renderer::Submit(m_Shader, m_VertexArray, transform);
    }

    for (int y = 0; y < 15; ++y)
    {
        for (int x = 0; x < 15; ++x)
        {
            glm::vec3 position = glm::vec3(x * 0.11f, y * 0.11f, 0.0f);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
            Grafix::Renderer::Submit(m_SquareShader, m_SquareVA, transform);
        }
    }

    Grafix::Renderer::EndScene();
}

void TestLayer::OnEvent(Grafix::Event& e)
{
}
