#include "wzpch.h"

#include "Wizzy/Renderer/RenderCommand.h"

#include "Wizzy/platform/OpenGL/GLRendererAPI.h"

namespace Wizzy {
    RendererAPI *RenderCommand::s_rendererAPI = new GLRendererAPI();
}
