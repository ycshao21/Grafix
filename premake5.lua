VULKAN = os.getenv("VULKAN_SDK")

workspace "Grafix"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

IncludeDir = {}
IncludeDir["Vulkan"] = "%{VULKAN}/Include"
IncludeDir["glfw"] = "Grafix/vendor/glfw/include"
IncludeDir["Glad"] = "Grafix/vendor/Glad/include"
IncludeDir["glm"] = "Grafix/vendor/glm"

Library = {}
Library["Vulkan"] = "%{VULKAN}/Lib/vulkan-1.lib"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
    include "Grafix/vendor/glfw"
    include "Grafix/vendor/Glad"
group ""


project "Grafix"
    location "Grafix"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "Grafix/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glm}"
    }

    links
    {
        "GLFW",
        "%{Library.Vulkan}",
        "opengl32.lib",
        "Glad",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "GF_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }
        
    filter "configurations:Debug"
        defines "GF_CONFIG_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "GF_CONFIG_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "GF_CONFIG_DIST"
        optimize "on"


project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Grafix/src",
        "Grafix/vendor",
        "Grafix/vendor/spdlog/include",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.Glad}"
    }

    links { "Grafix" }

    filter "system:windows"
        systemversion "latest"
        defines { "GF_PLATFORM_WINDOWS" }        

    filter "configurations:Debug"
        defines "GF_CONFIG_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "GF_CONFIG_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "GF_CONFIG_DIST"
        optimize "on"