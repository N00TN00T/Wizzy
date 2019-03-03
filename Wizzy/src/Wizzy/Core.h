#pragma once

#if defined(WZ_PLATFORM_WINDOWS)
	#ifdef WZ_EXPORT
		#define WZ_API __declspec(dllexport)
	#else
		#define WZ_API __declspec(dllimport)
	#endif
#elif defined(__GNUC__)
	#ifdef WZ_EXPORT
		#define WZ_API __attribute__((visibility("default")))
	#else
		#define WZ_API
	#endif
#else
	#define WZ_API
	#error Wizzy requires to be compiled with gcc for support on other platforms that windows
#endif

#define BIT(x) (1 << x)

#ifndef WZ_DISABLE_ASSERTS
	#ifdef WZ_PLATFORM_WINDOWS
		#define WZ_ASSERT(x, ...) if (!(x)) { WZ_CRITICAL("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); }
		#define WZ_CORE_ASSERT(x, ...) if (!(x)) { WZ_CORE_CRITICAL("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); }
	#elif defined(__GNUC__)
		#define WZ_ASSERT(x, ...) if (!(x)) { WZ_CRITICAL("Assertion failed: {0}", __VA_ARGS__); raise(SIGTRAP); }
		#define WZ_CORE_ASSERT(x, ...) if (!(x)) { WZ_CORE_CRITICAL("Assertion failed: {0}", __VA_ARGS__); raise(SIGTRAP); }
	#else
		#define WZ_ASSERT(x, ...)
		#define WZ_CORE_ASSERT(x, ...)
	#endif
#else
	#define WZ_ASSERT(x, ...) if (!(x)) { WZ_CRITICAL("Critical error: {0}", __VA_ARGS__); }
	#define WZ_CORE_ASSERT(x, ...) if (!(x)) { WZ_CORE_CRITICAL("Critical error: {0}", __VA_ARGS__); }
#endif

#define WZ_BIND_FN(fn) (std::bind(&fn, this, std::placeholders::_1))