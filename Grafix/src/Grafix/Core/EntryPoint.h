#pragma once
#include "Application.h"

#ifdef GF_PLATFORM_WINDOWS

    extern Grafix::Application* Grafix::CreateApplication();

    int main()
    {
        Grafix::Application* app = Grafix::CreateApplication();
        app->Run();
        delete app;
    }

#endif
