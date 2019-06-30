#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLRendererAPI.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"

namespace Wizzy {
    void GLRendererAPI::SetClearColor(float r, float g, float b, float a) {
        GL_CALL(glClearColor(r, g, b, a));
    }
    void GLRendererAPI::Clear()  {
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    void GLRendererAPI::DrawIndexed(const VertexArrayPtr& va, RenderMode mode) {
        GL_CALL(glDrawElements(GL_RenderModeToAPIRenderMode(mode), 
                                va->GetIndexBuffer()->GetCount(),
                                GL_UNSIGNED_INT, nullptr));
    }

    void GLRendererAPI::SetViewport(u32 x, u32 y, u32 w, u32 h) {
        GL_CALL(glViewport(x, y, w, h));
    }

    void GLRendererAPI::SetCullMode(CullMode mode) {
        if (mode != WZ_CULL_NONE) {
            if (!m_cullingEnabled) GL_CALL(glEnable(GL_CULL_FACE));
            GL_CALL(glCullFace(GL_CullModeToAPICullMode(mode)));
            m_cullingEnabled = true;
        } else {
            if (m_cullingEnabled) {
                GL_CALL(glDisable(GL_CULL_FACE));
            }
            m_cullingEnabled = false;
        }
    }
    void GLRendererAPI::ToggleDepthTesting(bool value) {
        if (value == m_depthTestingEnabled) return;

        if (value) {
            GL_CALL(glEnable(GL_DEPTH_TEST));
        } else {
            GL_CALL(glDisable(GL_DEPTH_TEST));
        }
    }
}
