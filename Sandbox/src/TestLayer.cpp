#include "TestLayer.h"

#include <glad/glad.h>

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
        std::string vertexSrc = R"(
            #version 450 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            uniform mat4 u_ViewProjection;

            out vec4 v_Color;

            void main()
            {
                v_Color = a_Color;
                gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentSrc = R"(
            #version 450 core

            layout(location = 0) out vec4 color;

            in vec4 v_Color;

            void main()
            {
                color = v_Color;
            }
        )";

        m_Shader = Grafix::Shader::Create(vertexSrc, fragmentSrc);
    }

    {
        m_SquareVA = Grafix::VertexArray::Create();

        // Vertex Buffer
        float vertices[4 * 3] = {
             0.7f,  0.7f, 0.0f,
            -0.7f,  0.7f, 0.0f,
            -0.7f, -0.7f, 0.0f,
             0.7f, -0.7f, 0.0f
        };
        auto squareVB = Grafix::VertexBuffer::Create(vertices, sizeof(vertices));

        squareVB->SetLayout({
            { Grafix::ShaderDataType::Float3, "a_Position" }
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
        auto squareIB = Grafix::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        m_SquareVA->SetIndexBuffer(squareIB);

        std::string squareVertexSrc = R"(
            #version 450 core

            layout(location = 0) in vec3 a_Position;

            uniform mat4 u_ViewProjection;

            void main()
            {
                gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
            }
        )";

        std::string squareFragmentSrc = R"(
            #version 450 core

            layout(location = 0) out vec4 color;

            void main()
            {
                color = vec4(0.4, 0.4, 0.4, 1.0);
            }
        )";
        
        m_SquareShader = Grafix::Shader::Create(squareVertexSrc, squareFragmentSrc);
    }
}

TestLayer::~TestLayer()
{
}

void TestLayer::OnUpdate(float ts)
{
    // Camera movement
    if (Grafix::Input::IsKeyPressed(Grafix::Key::A))
    {
        m_CameraPosition.x -= m_CameraMoveSpeed * ts;
    }
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::D))
    {
        m_CameraPosition.x += m_CameraMoveSpeed * ts;
    }

    if (Grafix::Input::IsKeyPressed(Grafix::Key::W))
    {
        m_CameraPosition.y += m_CameraMoveSpeed * ts;
    }
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::S))
    {
        m_CameraPosition.y -= m_CameraMoveSpeed * ts;
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

    // Reset camera
    if(Grafix::Input::IsKeyPressed(Grafix::Key::R))
    {
        m_CameraPosition = glm::vec3(0.0f);
        m_CameraRotation = 0.0f;
    }

    m_Camera.SetPosition(m_CameraPosition);
    m_Camera.SetRotation(m_CameraRotation);

    Grafix::Renderer::BeginScene(m_Camera);

    Grafix::Renderer::Submit(m_SquareShader, m_SquareVA);
    Grafix::Renderer::Submit(m_Shader, m_VertexArray);

    Grafix::Renderer::EndScene();
}

void TestLayer::OnEvent(Grafix::Event& e)
{
}
