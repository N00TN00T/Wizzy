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

#else

	/*#define ASSERT_MSG ("Critical error: '{0}'")

	#define EMMIT_ASSERTION_IF_FALSE(x, ...) if (!(x)) { WZ_CRITICAL(ASSERT_MSG, __VA_ARGS__); }

	#define WZ_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(x, __VA_ARGS__)
	#define WZ_CORE_ASSERT(x, ...) EMMIT_ASSERTION_IF_FALSE(x, __VA_ARGS__)*/


	#define WZ_ASSERT(x,...) x;
	#define WZ_CORE_ASSERT(x,...)x;

#endif

#define WZ_BIND_FN(fn) (std::bind(&fn, this, std::placeholders::_1))

#define BIT(x) (1 << x)

#define WZ_SHADER_ID_INVALID (0)
#define WZ_TEXTURE_ID_INVALID (0)

#define WZ_MAKE_VERSION(major, minor, patch) (std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch))
#define WZ_VERSION_MAJOR 0
#define WZ_VERSION_MINOR 1
#define WZ_VERSION_PATCH 0
#define WZ_VERSION	WZ_MAKE_VERSION(WZ_VERSION_MAJOR, WZ_VERSION_MINOR, WZ_VERSION_PATCH)

#define WZ_VERSION_SUM(major, minor, patch)		(major * 1000 + minor * 100 + patch)

#ifdef __GNUC__
	#define typestr(T) string(abi::__cxa_demangle(typeid(T).name(), 0, 0, 0))
#else
	#define typestr(T) string(typeid(T).name())
#endif

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

#define WZ_NULL_RESOURCE_HANDLE 	0

#define WZ_UNLOADED_TEXTURE_COLOR   ::Wizzy::Color::red
#define WZ_INVALID_TEXTURE_COLOR    ::Wizzy::Color::magenta

#define DEFAULT(t) t()

#define WZ_NULL_ENTITY_HANDLE nullptr

#define WZ_NS_BEGIN(ns) namespace ns {
#define WZ_NS_END }

// Serialization

#define WZ_TOKEN_HEADER							"!½WZ\n"
#define WZ_TOKEN_FOOTER							"\nWZ½!"
#define WZ_BYTE_TOKEN_BEGIN_FLOAT				((byte)4)
#define WZ_BYTE_TOKEN_BEGIN_INT					((byte)5)
#define WZ_BYTE_TOKEN_BEGIN_STRING				((byte)6)
#define WZ_BYTE_TOKEN_BEGIN_BOOL				((byte)7)
#define WZ_BYTE_TOKEN_NEXT_ITEM					((byte)21)
#define WZ_BYTE_TOKEN_FLAG_SEPARATOR			((byte)14)
#define WZ_BYTE_TOKEN_DATA_SEPARATOR			((byte)12)
#define WZ_BYTE_TOKEN_END						((byte)10)
#define WZ_IS_TYPE_BYTE(b)						(b == WZ_BYTE_TOKEN_BEGIN_FLOAT || b == WZ_BYTE_TOKEN_BEGIN_INT || b == WZ_BYTE_TOKEN_BEGIN_STRING || b == WZ_BYTE_TOKEN_BEGIN_BOOL)
#define WZ_IS_BYTE_TOKEN(b)						(WZ_IS_TYPE_BYTE(b) || b == WZ_BYTE_TOKEN_NEXT_ITEM || b == WZ_BYTE_TOKEN_END || b == WZ_BYTE_TOKEN_FLAG_SEPARATOR || b == WZ_BYTE_TOKEN_DATA_SEPARATOR)

#define IS_IMAGE_EXTENSION(x)	((string(x) == ".jpg" || string(x) == ".png" || string(x) == ".tga" || string(x) == ".bmp" || string(x) == ".psd" || string(x) == ".gif" || string(x) == ".hdr" || string(x) == ".pic") || \
								(string(x) == ".JPG" || string(x) == ".PNG" || string(x) == ".TGA" || string(x) == ".BMP" || string(x) == ".PSD" || string(x) == ".GIF" || string(x) == ".HDR" || string(x) == ".PIC") || \
								(string(x) == "JPG" || string(x) == "PNG" || string(x) == "TGA" || string(x) == "BMP" || string(x) == "PSD" || string(x) == "GIF" || string(x) == "HDR" || string(x) == "PIC") || \
								(string(x) == "jpg" || string(x) == "png" || string(x) == "tga" || string(x) == "bmp" || string(x) == "psd" || string(x) == "gif" || string(x) == "hdr" || string(x) == "pic"))
#define IS_SHADER_EXTENSION(x)	(string(x) == ".shader" || string(x) == ".SHADER"|| string(x) == "shader"|| string(x) == "SHADER")
#define IS_MODEL_EXTENSION(x)	((string(x) == ".obj" || string(x) == ".fbx" || string(x) == ".blend") ||\
								(string(x) == ".OBJ" || string(x) == ".FBX" || string(x) == ".BLEND") ||\
								(string(x) == "OBJ" || string(x) == "FBX" || string(x) == "BLEND") ||\
								(string(x) == "obj" || string(x) == "fbx" || string(x) == "blend"))
#define IS_SCRIPT_EXTENSION(x)	(string(x) == ".lua" || string(x) == ".LUA"|| string(x) == "lua"|| string(x) == "LUA")

#define _KB(x)		(1024 * x)
#define _MB(x)		(_KB(1000) * x)
#define _GB(x)		(_MB(1000) * x)

namespace Wizzy {

	enum ScriptEnum {
		SCRIPT_TYPE_NONE = -1,

		SCRIPT_TYPE_CONFIG,
		SCRIPT_TYPE_SYSTEM,
		SCRIPT_TYPE_COMPONENT
	};
	
}
