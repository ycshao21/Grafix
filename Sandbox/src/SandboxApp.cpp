#include "Grafix.h"
#include "Grafix/Core/EntryPoint.h"

#include "TestLayer.h"

class Sandbox : public Grafix::Application
{
public:
    Sandbox(const Grafix::ApplicationSpecification& spec)
        : Application(spec)
    {
        PushLayer(new TestLayer());
    }

    ~Sandbox()
    {
    }
};

Grafix::Application* Grafix::CreateApplication()
{
    ApplicationSpecification spec;
    spec.Name = "Grafix";
    spec.WindowWidth = 1600;
    spec.WindowHeight = 900;
    spec.VSync = true;

    return new Sandbox(spec);
}