


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
#include <bitset>

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

#include <portable-file-dialogs.h>

#include <ulib.hpp>

#include <imgui.h>


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
typedef uint8_t     byte;
typedef int8_t      sbyte;
typedef int8_t      int8;
typedef uint8_t     uint8;
typedef int16_t     int16;
typedef uint16_t    uint16;
typedef int32_t     int32;
typedef uint32_t    uint32;
typedef int64_t     int64;
typedef uint64_t    uint64;

typedef int8        s8;
typedef uint8       u8;
typedef int16       s16;
typedef uint16      u16;
typedef int32       s32;
typedef uint32      u32;
typedef int64       s64;
typedef uint64      u64;

typedef float       f32;
typedef double      f64;

const string& GetExecutablePath();
void SetExecutablePath(const string& p);

/* wizzy headers */

#include "Wizzy/Core/Core.h"
#include "Wizzy/Utilities/Log.h"

#include "Wizzy/Math/Vec.h"
#include "Wizzy/Math/Mat.h"

#include "Wizzy/Utilities/Bitset.h"