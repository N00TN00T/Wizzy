#pragma once

#include "Wizzy/Renderer/API.h"

namespace Wizzy {

    u32 GL_BufferUsageToAPIBufferUsage(BufferEnum v);

	size_t GL_ShaderDataTypeSize(ShaderDataType type);
	u32 GL_ShaderDataTypeElementCount(ShaderDataType type);

    int32 GL_ShaderDataTypeToAPIType(ShaderDataType sType);
    int32 GL_CullModeToAPICullMode(CullMode mode);
    int32 GL_RenderModeToAPIRenderMode(RenderMode mode);
    int32 GL_MaxTextureSlot();
    string GL_GetVersion();

    string GL_GetVendor();
}
