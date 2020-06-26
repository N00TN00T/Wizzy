#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLRendererAPI.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"
#include "Wizzy/platform/OpenGL/GLRenderTarget.h"

namespace Wizzy {

    color3 g_clearColor = COLOR_TRANSPARENT;

    void GLRendererAPI::SetClearColor(float r, float g, float b, float a) {
        if (g_clearColor.r != r && g_clearColor.g != g && g_clearColor.b != b && g_clearColor.a != a)
        {
            GL_CALL(glClearColor(r, g, b, a));
            g_clearColor.r = r; g_clearColor.g = g; g_clearColor.b = b; g_clearColor.a = a;
        }
    }

    const color3& GLRendererAPI::GetClearColor() const
    {
        return g_clearColor;
    }

    void GLRendererAPI::Clear()  {
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    }

    void GLRendererAPI::DrawIndexed(const VertexArrayPtr& va, u32 indexCount, RenderMode mode) {
        GL_CALL(glDrawElements(GL_RenderModeToAPIRenderMode(mode), indexCount, GL_UNSIGNED_INT, nullptr));
    }

    void GLRendererAPI::SetViewport(const Viewport& vp) {
        GL_CALL(glViewport(vp.x, vp.y, vp.width, vp.height));
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
            GL_CALL(glDepthFunc(GL_LESS));
        } else {
            GL_CALL(glDisable(GL_DEPTH_TEST));
        }

        m_depthTestingEnabled = value;
    }
    void GLRendererAPI::ToggleBlending(bool value) {
        if (value == m_blendingEnabled) return;

        if (value) {
            GL_CALL(glEnable(GL_BLEND));
            GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        } else {
            GL_CALL(glDisable(GL_BLEND));
        }

        m_blendingEnabled = value;
    }
}
