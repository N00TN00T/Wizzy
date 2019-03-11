#pragma once

/* std libraries */

#include <iostream>
#include <stdio.h>
#include <functional>
#include <memory>
#include <algorithm>
#include <utility>
#include <sstream>
#include <ostream>
#include <signal.h>

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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

/* wizzy headers */

#include "Wizzy/Log.h"
#include "Wizzy/utils.h"

/* Platform specific headers */
#ifdef WZ_PLATFORM_WINDOWS
    #include <Windows.h>
#endif

/* typedefs */

typedef std::string string;
typedef unsigned char byte;
typedef char int8;
typedef unsigned char uint8;
typedef uint8 u8;
typedef short int int16;
typedef unsigned short int uint16;
typedef uint16 u16;
typedef int int32;
typedef unsigned int uint32;
typedef uint32 u32;
typedef long long int64;
typedef unsigned long long uint64;
typedef uint64 u64;

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;