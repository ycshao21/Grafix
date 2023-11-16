#include "pch.h"
#include "Application.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Grafix/Renderer/Renderer.h"

namespace Grafix
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification& spec)
    {
        s_Instance = this;

        WindowSpecification windowSpec;
        windowSpec.Title = spec.Name;
        windowSpec.Width = spec.WindowWidth;
        windowSpec.Height = spec.WindowHeight;
        windowSpec.VSync = spec.VSync;

        m_Window = Window::Create(windowSpec);
        m_Window->Init();
        m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });

        Renderer::Init();

        // TEMP: Move these into Renderer
        {
            m_TriVertexArray = VertexArray::Create();

            // Vertex Buffer
            float vertices[3 * 7] = {
                -0.5f, -0.5f, 0.0f, 0.85f, 0.0f,  0.85f, 1.0f,
                 0.5f, -0.5f, 0.0f, 0.0f,  0.85f, 0.85f, 1.0f,
                 0.0f,  0.5f, 0.0f, 0.85f, 0.85f, 0.0f,  1.0f
            };
            m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));

            VertexBufferLayout layout = {
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float4, "a_Color" }
            };
            m_VertexBuffer->SetLayout(layout);
            m_TriVertexArray->AddVertexBuffer(m_VertexBuffer);

            // Index Buffer
            uint32_t indices[3] = { 0, 1, 2 };
            m_IndexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
            m_TriVertexArray->SetIndexBuffer(m_IndexBuffer);

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

            m_Shader = CreateShared<Shader>(vertexSrc, fragmentSrc);
        }

        m_SquareVA = VertexArray::Create();

        // Vertex Buffer
        float vertices[4 * 3] = {
             0.7f,  0.7f, 0.0f,
            -0.7f,  0.7f, 0.0f,
            -0.7f, -0.7f, 0.0f,
             0.7f, -0.7f, 0.0f
        };
        Shared<VertexBuffer> squareVB = VertexBuffer::Create(vertices, sizeof(vertices));

        squareVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
        Shared<IndexBuffer> squareIB = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
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
                color = vec4(0.8, 0.6, 0.1, 1.0);
            }
        )";
        
        m_SquareShader = CreateShared<Shader>(squareVertexSrc, squareFragmentSrc);
    }

    Application::~Application()
    {
        Renderer::Shutdown();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowClose(e); });

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
            {
                break;
            }
        }
    }

    void Application::Run()
    {
        while(m_IsRunning)
        {
            // TODO: Make glfwGetTime() disappear
            float time = (float)glfwGetTime();
            float ts = time - m_LastFrameTime;
            m_LastFrameTime = time;
            
            // TEMP
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_SquareShader->Bind();
            m_SquareVA->Bind();
            glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

            m_Shader->Bind();
            m_TriVertexArray->Bind();
            glDrawElements(GL_TRIANGLES, m_TriVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

            for(Layer* layer : m_LayerStack)
                layer->OnUpdate(ts);

            m_Window->OnUpdate();
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_IsRunning = false;
        return false;
    }
}