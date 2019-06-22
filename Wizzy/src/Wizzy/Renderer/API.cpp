#include "wzpch.h"

#include "Wizzy/Renderer/API.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"
#include "Wizzy/Renderer/Renderer.h"

namespace Wizzy {
    int32 ShaderDataTypeToAPIType(ShaderDataType sType) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API_NONE: break;
            case RendererAPI::API_OPENGL: return GLShaderDataTypeToAPIType(sType);
        }

        WZ_CORE_ASSERT(false, "Invalid renderer API selected ({0})", std::to_string((int32)sType));
        return 0;
    }
}
