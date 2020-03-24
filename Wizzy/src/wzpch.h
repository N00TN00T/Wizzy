
/* std libraries */
#include <iostream>
#include <stdio.h>
#include <functional>
#include <memory>
#include <algorithm>
#include <utility>
#include <sstream>
#include <ostream>
#include <fstream>
#include <signal.h>
#include <typeinfo>
#include <assert.h>
#include <new>
#include <cmath>
#include <stdarg.h>
#include <filesystem>
#include <chrono>
#include <cstring>
#include <cstdint>
#include <variant>
#include <mutex>
#include <future>
#include <condition_variable>
#include <atomic>

/* std data structures */
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
#include <deque>
#include <list>
#include <array>

/* external libraries */

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

/* wizzy headers */

#include "Wizzy/Core.h"
#include "Wizzy/Log.h"
#include "Wizzy/Color.h"

#include <ulib.hpp>

/* Platform/Compiler specific headers */
//#ifdef WZ_PLATFORM_WINDOWS
//namespace WinAPI {
//    #include <Windows.h>
//}
//#endif
//#ifdef __GNUC__
//    #include <cxxabi.h>
//#endif

/* typedefs */

typedef std::string string;
typedef uint8_t byte;
typedef int8_t sbyte;
typedef int8_t int8;
typedef uint8_t uint8;
typedef uint8 u8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef uint16 u16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef uint32 u32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef uint64 u64;

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;

const string& GetExecutablePath();
void SetExecutablePath(const string& p);