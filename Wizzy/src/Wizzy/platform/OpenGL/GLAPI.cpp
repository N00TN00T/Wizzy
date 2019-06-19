#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/GLAPI.h"

namespace Wizzy {
    int32 GLShaderDataTypeToAPIType(ShaderDataType sType) {
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

        WZ_CORE_ASSERT(false, "Unimplemented shader data type");
        return 0;
    }
}
