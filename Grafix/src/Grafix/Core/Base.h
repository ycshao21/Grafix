#pragma once

#include "Logging.h"

#include <memory>

#ifdef GF_CONFIG_DEBUG
    #define GF_ENABLE_ASSERTS
#endif


#ifdef GF_ENABLE_ASSERTS
	#define GF_CORE_ASSERT(x, ...) { if(!(x)) { GF_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define GF_ASSERT(x, ...) { if(!(x)) { GF_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	
    #ifdef GF_PLATFORM_WINDOWS
        #define GF_VK_CHECK(x, ...) { if(x != VK_SUCCESS) { GF_CORE_ERROR("Vulkan Error: {0}", __VA_ARGS__); __debugbreak(); } }
    #endif
#else
	#define GF_CORE_ASSERT(x, ...)
	#define GF_ASSERT(x, ...)

    #ifdef GF_PLATFORM_WINDOWS
        #define GF_VK_CHECK(x, ...)
    #endif
#endif

#define BIT(x) (1 << x)

namespace Grafix
{
	template<typename T>
	using Unique = std::unique_ptr<T>;

	template<typename T, typename... Args>
	constexpr Unique<T> CreateUnique(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Shared = std::shared_ptr<T>;

	template<typename T, typename... Args>
	constexpr Shared<T> CreateShared(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
