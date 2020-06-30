#pragma once

#if defined(WZ_EXTENSIONS_ENABLE_ALL) || defined(WZ_EXTENSIONS_ENABLE_PHYSICS2D)

#include "Wizzy/Extensions/Physics2D/Rigidbody2D.h"

namespace Wizzy {
    namespace Phys2D = Physics2D;
}

#else
    #error WZ_EXTENSIONS_ENABLE_PHYSICS2D must be defined to use the Wizzy Physics2D Extension
#endif