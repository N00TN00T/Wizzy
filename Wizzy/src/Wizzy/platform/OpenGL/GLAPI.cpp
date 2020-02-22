#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"

namespace Wizzy {
	u32 GL_BufferUsageToAPIBufferUsage(BufferEnum v)
	{
		switch (v)
		{
            case WZ_BUFFER_USAGE_STREAM_DRAW:   return GL_STREAM_DRAW; break;
            case WZ_BUFFER_USAGE_STREAM_READ:   return GL_STREAM_READ; break;
            case WZ_BUFFER_USAGE_STREAM_COPY:   return GL_STREAM_COPY; break;
            case WZ_BUFFER_USAGE_STATIC_DRAW:   return GL_STATIC_DRAW; break;
            case WZ_BUFFER_USAGE_STATIC_READ:   return GL_STATIC_READ; break;
            case WZ_BUFFER_USAGE_STATIC_COPY:   return GL_STATIC_COPY; break;
            case WZ_BUFFER_USAGE_DYNAMIC_DRAW:  return GL_DYNAMIC_DRAW; break;
            case WZ_BUFFER_USAGE_DYNAMIC_READ:  return GL_DYNAMIC_READ; break;
            case WZ_BUFFER_USAGE_DYNAMIC_COPY:  return GL_DYNAMIC_COPY; break;
		}

        WZ_CORE_ASSERT(false, "Unimplemented buffer usage");
        return 0;
	}
	size_t GL_ShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::NONE:      break;
		case ShaderDataType::FLOAT1:    return 1 * 4;
		case ShaderDataType::FLOAT2:    return 2 * 4;
		case ShaderDataType::FLOAT3:    return 3 * 4;
		case ShaderDataType::FLOAT4:    return 4 * 4;
		case ShaderDataType::MAT3:      return 9 * 4;
		case ShaderDataType::MAT4:      return 16 * 4;
		case ShaderDataType::INT1:      return 1 * 4;
		case ShaderDataType::INT2:      return 2 * 4;
		case ShaderDataType::INT3:      return 3 * 4;
		case ShaderDataType::INT4:      return 4 * 4;
		case ShaderDataType::TEXTURE2D:   return GL_ShaderDataTypeSize(ShaderDataType::INT1);
		case ShaderDataType::BOOL:      return 1;
		}

		WZ_CORE_ASSERT(false, "Unimplemented shader data type");
		return 0;
	}

	u32 GL_ShaderDataTypeElementCount(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::NONE:      break;
		case ShaderDataType::FLOAT1:    return 1;
		case ShaderDataType::FLOAT2:    return 2;
		case ShaderDataType::FLOAT3:    return 3;
		case ShaderDataType::FLOAT4:    return 4;
		case ShaderDataType::MAT3:      return 9;
		case ShaderDataType::MAT4:      return 16;
		case ShaderDataType::INT1:      return 1;
		case ShaderDataType::INT2:      return 2;
		case ShaderDataType::INT3:      return 3;
		case ShaderDataType::INT4:      return 4;
		case ShaderDataType::TEXTURE2D:   return GL_ShaderDataTypeElementCount(ShaderDataType::INT1);
		case ShaderDataType::BOOL:      return 1;
		}

		WZ_CORE_ASSERT(false, "Unimplemented shader data type");
		return 0;
	}

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
			case ShaderDataType::TEXTURE2D:	return GL_SAMPLER_2D;
            case ShaderDataType::BOOL:      return GL_BOOL;
        }

        WZ_CORE_ASSERT(false, "GL Unimplemented shader data type");
        return 0;
    }

    int32 GL_CullModeToAPICullMode(CullMode mode) {
        switch (mode) {
            case WZ_CULL_BACK:   return GL_BACK;
            case WZ_CULL_FRONT:  return GL_FRONT;
            case WZ_CULL_NONE:   break;
        }
        WZ_CORE_ASSERT(false, "GL Unimplemented culling mode");
        return 0;
    }
    int32 GL_RenderModeToAPIRenderMode(RenderMode mode) {
        switch (mode) {
            case WZ_RENDER_MODE_LINES:      return GL_LINES;
            case WZ_RENDER_MODE_TRIANGLES:  return GL_TRIANGLES;
            case WZ_RENDER_MODE_NONE:       break;
        }
        WZ_CORE_ASSERT(false, "GL Unimplemented render mode");
        return 0;
    }
    int32 GL_MaxTextureSlot() {
        int32 c = 0;
        GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &c));
        return c - 1;
    }
    string GL_GetVersion() {
      string _glVersion = reinterpret_cast< char const* >(glGetString(GL_VERSION));
      string _glslVersion = reinterpret_cast< char const* >(glGetString(GL_SHADING_LANGUAGE_VERSION));

      return "[GL] " + _glVersion + " | [GLSL] " + _glslVersion;
    }

    string GL_GetVendor() {
        return reinterpret_cast< char const* >(glGetString(GL_VENDOR))
                + string(" (")
                + reinterpret_cast< char const* >(glGetString(GL_RENDERER))
                + string(")");
    }
}
