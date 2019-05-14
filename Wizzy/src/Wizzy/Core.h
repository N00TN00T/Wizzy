#pragma once

#if !defined(WZ_PLATFORM_WINDOWS) && !defined(WZ_PLATFORM_LINUX)
	#error Wizzy is currently only supported on Windows and Linux
#endif

#ifdef __GNUC__
	#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#else
	#define __FILENAME__  __FUNCTION__
#endif

#ifdef WZ_PLATFORM_WINDOWS
	#define WZ_BREAK __debugbreak()
#elif defined (__GNUC__)
	#define WZ_BREAK __builtin_trap()
#else
	#define WZ_BREAK signal(SIGTRAP)
#endif

#define WZ_ERR_NONE 0
#define WZ_ERR_INIT_FAILURE 1

#define WZ_ERR_STRING(e) __WZ_ERROR_STRINGS.at(e)

#ifndef WZ_DISABLE_ASSERTS

	#define ASSERT_MSG (("Assertion failed: '{0}' in " + string(__FILENAME__) + ":" + std::to_string(__LINE__)).c_str())

	#define WZ_ASSERT(x, ...) if (!(x)) { WZ_CRITICAL(ASSERT_MSG, __VA_ARGS__); WZ_BREAK; }
	#define WZ_CORE_ASSERT(x, ...) if (!(x)) { WZ_CORE_CRITICAL(ASSERT_MSG, __VA_ARGS__); WZ_BREAK; }

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

#define WZ_SHADER_ID_INVALID (0)
#define WZ_TEXTURE_ID_INVALID (0)

#define WZ_MAKE_VERSION(major, minor, patch) (std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch))
#define WZ_VERSION_MAJOR 0
#define WZ_VERSION_MINOR 1
#define WZ_VERSION_PATCH 1
#define WZ_VERSION	WZ_MAKE_VERSION(WZ_VERSION_MAJOR, WZ_VERSION_MINOR, WZ_VERSION_PATCH)

const std::unordered_map<int, std::string> __WZ_ERROR_STRINGS = {
	{ WZ_ERR_NONE, "no error" },
	{ WZ_ERR_INIT_FAILURE, "Initialization failure" },
};


#ifdef __GNUC__
	#define typestr(T) abi::__cxa_demangle(typeid(T).name(), 0, 0, 0)
#else
	#define typestr(T) typeid(T).name()
#endif

#ifdef __GNUC__
	#define BASE_DIR ""
#elif defined (_MSC_VER)
	#define BASE_DIR IsDebuggerPresent() ? "../" : "../../../"
#else
	#define BASE_DIR "../../../"
#endif

#define WZ_NULL_ENTITY_HANDLE nullptr