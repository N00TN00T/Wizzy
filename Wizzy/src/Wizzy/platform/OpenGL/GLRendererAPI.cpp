#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLRendererAPI.h"

namespace Wizzy {
    void GLRendererAPI::SetClearColor(float r, float g, float b, float a) {
        GL_CALL(glClearColor(r, g, b, a));
    }
    void GLRendererAPI::Clear()  {
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT*/));
    }

    void GLRendererAPI::DrawIndexed(const VertexArrayPtr& va) {
        GL_CALL(glDrawElements(GL_TRIANGLES, va->GetIndexBuffer()->GetCount(),
                                GL_UNSIGNED_INT, nullptr));
    }

    void GLRendererAPI::SetViewport(u32 x, u32 y, u32 w, u32 h) {
        GL_CALL(glViewport(x, y, w, h));
    }
}
