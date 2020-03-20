#include "wzpch.h"

#include "Wizzy/Renderer/API.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"
#include "Wizzy/Renderer/Renderer.h"

namespace Wizzy {
    u32 BufferUsageToAPIBufferUsage(BufferEnum v)
    {
        CREATE_BY_API(GL_BufferUsageToAPIBufferUsage(v));

        return 0;
    }
    size_t ShaderDataTypeSize(ShaderDataType type) {
		 
		CREATE_BY_API(GL_ShaderDataTypeSize(type));

		return 0;
	}

	u32 ShaderDataTypeElementCount(ShaderDataType type) {

		CREATE_BY_API(GL_ShaderDataTypeElementCount(type));

		return 0;
	}

    int32 ShaderDataTypeToAPIType(ShaderDataType sType) {

        CREATE_BY_API(GL_ShaderDataTypeToAPIType(sType));

        return 0;
    }

    int32 CullModeToAPICullMode(CullMode mode) {

        CREATE_BY_API(GL_CullModeToAPICullMode(mode));

        return 0;
    }

    int32 RenderModeToAPIRenderMode(RenderMode mode) {

        CREATE_BY_API(GL_RenderModeToAPIRenderMode(mode));

        return 0;
    }

    int32 MaxTextureSlot() {

        CREATE_BY_API(GL_MaxTextureSlot());

        return 0;
    }

    int32 TextureChannelsToAPIFormat(u8 channels)
    {
        CREATE_BY_API(GL_TextureChannelsToAPIFormat(channels));

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
