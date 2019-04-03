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

#undef WZ_API
#define WZ_API

#ifdef __GNUC__
	#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#else
	#define __FILENAME__  __FUNCTION__
#endif

#define WZ_ERR_NONE 0
#define WZ_ERR_INIT_FAILURE 1

#define WZ_ERR_STRING(e) __WZ_ERROR_STRINGS.at(e)

#ifndef WZ_DISABLE_ASSERTS

	#define ASSERT_MSG (("Assertion failed: '{0}' in " + string(__FILENAME__) + ":" + std::to_string(__LINE__)).c_str())
	
	#define EMMIT_ASSERTION_IF_FALSE(fn, x, ...) if (!(x)) { WZ_CRITICAL(ASSERT_MSG, __VA_ARGS__); fn; }

	#ifdef WZ_PLATFORM_WINDOWS
		#define WZ_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(__debugbreak(), x, __VA_ARGS__)
		#define WZ_CORE_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(__debugbreak(), x, __VA_ARGS__)
	#elif defined(__GNUC__)
		#define WZ_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(__builtin_trap(), x, __VA_ARGS__)
		#define WZ_CORE_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(__builtin_trap(), x, __VA_ARGS__)
	#else
		#define WZ_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(signal(SIGTRAP), x, __VA_ARGS__)
		#define WZ_CORE_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(signal(SIGTRAP), x, __VA_ARGS__)
	#endif

	#define WZ_ASSERT_ERROR(e) WZ_CORE_ASSERT(e == WZ_ERR_NONE, "Wizzy error! (code " + std::to_string(e) + "): '" + WZ_ERR_STRING(e) + "'")

#else

	#define ASSERT_MSG ("Critical error: '{0}'")
	
	#define EMMIT_ASSERTION_IF_FALSE(x, ...) if (!(x)) { WZ_CRITICAL(ASSERT_MSG, __VA_ARGS__); }

	#define WZ_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(x, __VA_ARGS__)
	#define WZ_CORE_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(x, __VA_ARGS__)

	#define WZ_ASSERT_ERROR(e) if (e != WZ_ERR_NONE) WZ_CRITICAL("Critical error! Code {0}: {1}", e, WZ_ERR_STRING(e))

#endif

#define WZ_BIND_FN(fn) (std::bind(&fn, this, std::placeholders::_1))

#define BIT(x) (1 << x)

const std::unordered_map<int, std::string> __WZ_ERROR_STRINGS = {
	{ WZ_ERR_NONE, "no error" },
	{ WZ_ERR_INIT_FAILURE, "Initialization failure" },
};
