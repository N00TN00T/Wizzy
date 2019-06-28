#pragma once

#include "Wizzy/Renderer/API.h"

namespace Wizzy {
    int32 GL_ShaderDataTypeToAPIType(ShaderDataType sType);
    int32 GL_CullModeToAPICullMode(CullMode mode);
    int32 GL_MaxTextureSlot();
    string GL_GetVersion();

    string GL_GetVendor();
}
