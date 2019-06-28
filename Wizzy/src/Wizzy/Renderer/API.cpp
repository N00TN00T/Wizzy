#include "wzpch.h"

#include "Wizzy/Renderer/API.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"
#include "Wizzy/Renderer/Renderer.h"

namespace Wizzy {
    int32 ShaderDataTypeToAPIType(ShaderDataType sType) {

        CREATE_BY_API(GL_ShaderDataTypeToAPIType(sType));

        return 0;
    }

    int32 CullModeToAPICullMode(CullMode mode) {

        CREATE_BY_API(GL_CullModeToAPICullMode(mode));

        return 0;
    }

    int32 MaxTextureSlot() {
        
        CREATE_BY_API(GL_MaxTextureSlot());

        return 0;
    }

    string APIVersion() {

        CREATE_BY_API(GL_GetVersion());

        return "";
    }

    string APIVendor() {

        CREATE_BY_API(GL_GetVendor());

        return "";
    }
}
