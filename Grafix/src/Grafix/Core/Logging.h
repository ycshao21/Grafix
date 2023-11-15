#pragma once

#include <spdlog/spdlog.h>

#include <memory>

namespace Grafix
{
    class Logging final
    {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

// Core logging
#define GF_CORE_DEBUG(...) ::Grafix::Logging::GetCoreLogger()->debug(__VA_ARGS__)
#define GF_CORE_TRACE(...) ::Grafix::Logging::GetCoreLogger()->trace(__VA_ARGS__)
#define GF_CORE_INFO(...) ::Grafix::Logging::GetCoreLogger()->info(__VA_ARGS__)
#define GF_CORE_WARN(...) ::Grafix::Logging::GetCoreLogger()->warn(__VA_ARGS__)
#define GF_CORE_ERROR(...) ::Grafix::Logging::GetCoreLogger()->error(__VA_ARGS__)

// Client logging
#define GF_DEBUG(...) ::Grafix::Logging::GetClientLogger()->debug(__VA_ARGS__)
#define GF_TRACE(...) ::Grafix::Logging::GetClientLogger()->trace(__VA_ARGS__)
#define GF_INFO(...) ::Grafix::Logging::GetClientLogger()->info(__VA_ARGS__)
#define GF_WARN(...) ::Grafix::Logging::GetClientLogger()->warn(__VA_ARGS__)
#define GF_ERROR(...) ::Grafix::Logging::GetClientLogger()->error(__VA_ARGS__)
