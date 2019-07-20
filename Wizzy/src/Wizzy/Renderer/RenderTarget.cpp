#include "wzpch.h"

#include "Wizzy/Renderer/RenderTarget.h"
#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/platform/OpenGL/GLRenderTarget.h"

namespace Wizzy {
    RenderTarget* RenderTarget::Create(u32 width, u32 height) {
        CREATE_BY_API(new GLRenderTarget(width, height));
    }
}
