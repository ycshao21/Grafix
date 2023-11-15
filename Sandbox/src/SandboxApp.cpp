#include "Grafix.h"
#include "Grafix/Core/EntryPoint.h"

#include "SandboxLayer.h"

class Sandbox : public Grafix::Application
{
public:
    Sandbox(const Grafix::ApplicationSpecification& spec)
        : Application(spec)
    {
        PushLayer(new SandboxLayer());
    }

    ~Sandbox()
    {
    }
};

Grafix::Application* Grafix::CreateApplication()
{
    ApplicationSpecification spec("Sandbox", 1600, 900);
    return new Sandbox(spec);
}