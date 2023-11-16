#include "TestLayer.h"

TestLayer::TestLayer()
{
    {
        m_VertexArray = Grafix::VertexArray::Create();

        // Vertex Buffer
        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.85f, 0.0f,  0.85f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.0f,  0.85f, 0.85f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.85f, 0.85f, 0.0f,  1.0f
        };
        m_VertexBuffer = Grafix::VertexBuffer::Create(vertices, sizeof(vertices));

        Grafix::VertexBufferLayout layout = {
            { Grafix::ShaderDataType::Float3, "a_Position" },
            { Grafix::ShaderDataType::Float4, "a_Color" }
        };
        m_VertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        // Index Buffer
        uint32_t indices[3] = { 0, 1, 2 };
        m_IndexBuffer = Grafix::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        // Shader
        std::string vertexSrc = R"(
            #version 450

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            out vec3 v_Position;
            out vec4 v_Color;

            void main()
            {
                v_Position = a_Position;
                v_Color = a_Color;
                gl_Position = vec4(a_Position, 1.0);
            }
        )";

            std::string fragmentSrc = R"(
            #version 450

            layout(location = 0) out vec4 color;
            in vec3 v_Position;
            in vec4 v_Color;

            void main()
            {
                color = vec4(v_Position * 0.5 + 0.5, 1.0);
                color = v_Color;
            }
        )";

        m_Shader = Grafix::CreateShared<Grafix::Shader>(vertexSrc, fragmentSrc);
    }

    m_SquareVA = Grafix::VertexArray::Create();

    // Vertex Buffer
    float vertices[4 * 3] = {
         0.7f,  0.7f, 0.0f,
        -0.7f,  0.7f, 0.0f,
        -0.7f, -0.7f, 0.0f,
         0.7f, -0.7f, 0.0f
    };
    Grafix::Shared<Grafix::VertexBuffer> squareVB = Grafix::VertexBuffer::Create(vertices, sizeof(vertices));

    squareVB->SetLayout({
        { Grafix::ShaderDataType::Float3, "a_Position" }
    });
    m_SquareVA->AddVertexBuffer(squareVB);

    uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
    Grafix::Shared<Grafix::IndexBuffer> squareIB = Grafix::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
    m_SquareVA->SetIndexBuffer(squareIB);

    std::string squareVertexSrc = R"(
        #version 450

        layout(location = 0) in vec3 a_Position;

        void main()
        {
            gl_Position = vec4(a_Position, 1.0);
        }
    )";

    std::string squareFragmentSrc = R"(
        #version 450

        layout(location = 0) out vec4 color;

        void main()
        {
            color = vec4(0.6, 0.6, 0.6, 1.0);
        }
    )";
    
    m_SquareShader = Grafix::CreateShared<Grafix::Shader>(squareVertexSrc, squareFragmentSrc);
}

TestLayer::~TestLayer()
{
}

void TestLayer::OnUpdate(float ts)
{
    Grafix::Renderer::BeginScene();

    m_SquareShader->Bind();
    Grafix::Renderer::Submit(m_SquareVA);

    m_Shader->Bind();
    Grafix::Renderer::Submit(m_VertexArray);

    Grafix::Renderer::EndScene();
}
