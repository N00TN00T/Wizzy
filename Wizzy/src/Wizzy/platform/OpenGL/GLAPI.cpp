#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/GLAPI.h"

namespace Wizzy {
    int32 GL_ShaderDataTypeToAPIType(ShaderDataType sType) {
        switch (sType) {
            case ShaderDataType::NONE:      break;
            case ShaderDataType::FLOAT1:    return GL_FLOAT;
            case ShaderDataType::FLOAT2:    return GL_FLOAT;
            case ShaderDataType::FLOAT3:    return GL_FLOAT;
            case ShaderDataType::FLOAT4:    return GL_FLOAT;
            case ShaderDataType::MAT3:      return GL_FLOAT;
            case ShaderDataType::MAT4:      return GL_FLOAT;
            case ShaderDataType::INT1:      return GL_INT;
            case ShaderDataType::INT2:      return GL_INT;
            case ShaderDataType::INT3:      return GL_INT;
            case ShaderDataType::INT4:      return GL_INT;
            case ShaderDataType::BOOL:      return GL_BOOL;
        }

        WZ_CORE_ASSERT(false, "GL Unimplemented shader data type");
        return 0;
    }

    int32 GL_CullModeToAPICullMode(CullMode mode) {
        switch (mode) {
            case WZ_CULL_BACK:   return GL_BACK;
            case WZ_CULL_FRONT:  return GL_FRONT;
            case WZ_CULL_NONE:   return 0;
        }
        WZ_CORE_ASSERT(false, "GL Unimplemented culling mode");
        return 0;
    }
    int32 GL_MaxTextureSlot() {
        return GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1;
    }
    string GL_GetVersion() {
        return reinterpret_cast< char const* >(glGetString(GL_VERSION));
    }

    string GL_GetVendor() {
        return reinterpret_cast< char const* >(glGetString(GL_VENDOR))
                + string(" (")
                + reinterpret_cast< char const* >(glGetString(GL_RENDERER))
                + string(")");
    }
}