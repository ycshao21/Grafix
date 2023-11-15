#pragma once
#include "Application.h"

#include "Logging.h"

#ifdef GF_PLATFORM_WINDOWS

extern Grafix::Application* Grafix::CreateApplication();

int main()
{
    Grafix::Logging::Init();

    Grafix::Application* app = Grafix::CreateApplication();
    app->Run();
    delete app;
}

#endif
