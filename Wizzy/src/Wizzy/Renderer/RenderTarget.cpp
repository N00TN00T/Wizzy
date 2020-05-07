#include "wzpch.h"

#include "Wizzy/Renderer/RenderTarget.h"
#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/platform/OpenGL/GLRenderTarget.h"

namespace Wizzy {
    RenderTarget* RenderTarget::Create(s32 width, s32 height, s32 channels) {
        CREATE_BY_API(new GLRenderTarget(width, height, channels));
    }
    Resource* RenderTarget::Create(const ResData& data, const PropertyTable& props)
    {
        CREATE_BY_API(new GLRenderTarget(data, props));
    }
    const PropertyTable& RenderTarget::GetTemplateProps()
    {
        static PropertyTable pl;
        return pl;
    }
}
