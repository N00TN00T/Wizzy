#pragma once

#ifdef WZ_PLATFORM_WINDOWS
	#ifdef WZ_EXPORT
		#define WZ_API __declspec(dllexport)
	#else
		#define WZ_API __declspec(dllimport)
	#endif
#else
	#ifdef WZ_EXPORT
		#define WZ_API __attribute__((visibility("default")))
	#else
		#define WZ_API
	#endif
#endif

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

	#define ASSERT_MSG(x) (("Assertion failed for expression '" + string(#x) + "': '{0}' in " + std::string(__FILENAME__) + ":" + std::to_string(__LINE__)).c_str())

	#define WZ_ASSERT(x, ...) if (!(x)) { WZ_CRITICAL(ASSERT_MSG(x), __VA_ARGS__); WZ_BREAK; }
	#define WZ_CORE_ASSERT(x, ...) if (!(x)) { WZ_CORE_CRITICAL(ASSERT_MSG(x), __VA_ARGS__); WZ_BREAK; }
    #define WZ_LUA_ASSERT(x, msg) if (!(x)) { throw std::exception(msg); }

#else

	#define ASSERT_MSG ("Critical error: '{0}'")

	#define EMMIT_ASSERTION_IF_FALSE(x, ...) if (!(x)) { WZ_CRITICAL(ASSERT_MSG, __VA_ARGS__); }

	#define WZ_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(x, __VA_ARGS__)
	#define WZ_CORE_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(x, __VA_ARGS__)

#endif

#define WZ_BIND_FN(fn) (std::bind(&fn, this, std::placeholders::_1))

#define BIT(x) (1 << x)

#define WZ_SHADER_ID_INVALID (0)
#define WZ_TEXTURE_ID_INVALID (0)

#define WZ_MAKE_VERSION(major, minor, patch) (std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch))
#define WZ_VERSION_MAJOR 0
#define WZ_VERSION_MINOR 2
#define WZ_VERSION_PATCH 1
#define WZ_VERSION	WZ_MAKE_VERSION(WZ_VERSION_MAJOR, WZ_VERSION_MINOR, WZ_VERSION_PATCH)

#ifdef __GNUC__
	#define typestr(T) string(abi::__cxa_demangle(typeid(T).name(), 0, 0, 0))
#else
	#define typestr(T) string(typeid(T).name())
#endif


/* DECIDING THE BASE DIRECTORY */
#ifndef WZ_CONFIG_DIST
    #ifdef __GNUC__
        #ifdef __CODELITE__
            #define BASE_DIR "../"
        #elif defined(__XCODE__)
            #error Wizzy is not yet set up for compiling with xcode
        #else
            #define BASE_DIR ""
        #endif /* ifdef __CODELITE__ */
    #elif defined (_MSC_VER)
        #define BASE_DIR IsDebuggerPresent() ? "../" : "../../../"
    #else
        #define BASE_DIR "../../../"
    #endif /* defined (_MSC_VER) */
#else
    #error Wizzy is not set up for distribution yet
#endif /* ifndef WZ_CONFIG_DIST */

#define WZ_NULL_ENTITY_HANDLE nullptr

#define WZ_LUA_NEW_SCRIPT_CODE R"(-- This is a lua script for Wizzy)"

#define IS_INT(T)       typestr(T) == typestr(u8) || typestr(T) == typestr(int8) || typestr(T) == typestr(u16) || typestr(T) == typestr(int16) || typestr(T) == typestr(u32) || typestr(T) == typestr(int32) || typestr(T) == typestr(u64) || typestr(T) == typestr(int64)

#define IS_DECIMAL(T)   typestr(T) == typestr(float) || typestr(T) == typestr(double)

#define IS_STRING(T)    typestr(T) == typestr(string) || typestr(T) == typestr(char*)

#define IS_BOOL(T)      typestr(T) == typestr(bool)

#define CREATE_BY_API(glfn) switch (Renderer::GetAPI()) { \
	case RendererAPI::API_NONE: WZ_CORE_ERROR("No renderer API is selected"); break; \
	case RendererAPI::API_OPENGL: return glfn; break; \
} \
WZ_CORE_ASSERT(false, "Invalid renderer API selected"); \

namespace Wizzy {
    enum BufferUsage {
        WZ_STATIC = 0, WZ_DYNAMIC = 1, WZ_STREAM = 2
    };

    class Application;
    typedef Wizzy::Application* (*create_application_fn)();
}
