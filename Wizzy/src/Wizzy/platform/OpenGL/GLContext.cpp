#include "wzpch.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLContext.h"

void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            GLuint id, 
                            GLenum severity, 
                            GLsizei length, 
                            const GLchar *message, 
                            void *userParam)
{
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        {
            WZ_CORE_TRACE("OpenGL Notification:\n{0}\nid : {1}", message, id);
            break;
        }
        case GL_DEBUG_SEVERITY_LOW:
        {
            WZ_CORE_WARN("OpenGL low warning:\n{0}\nid : {1}", message, id);
            break;
        }
        case GL_DEBUG_SEVERITY_MEDIUM:
        {
            WZ_CORE_ERROR("OpenGL error:\n{0}\nid : {1}", message, id);
            break;
        }
        case GL_DEBUG_SEVERITY_HIGH:
        {
            WZ_CORE_CRITICAL("OpenGL critical error:\n{0}\nid : {1}", message, id);
            WZ_BREAK;
            break;
        }
    }
}

namespace Wizzy {
    GLContext::GLContext(GLFWwindow *windowHandle)
        : m_windowHandle(windowHandle) {
        WZ_CORE_ASSERT(windowHandle, "Window handle is null");

        
    }

    void GLContext::Init() {
        this->MakeCurrent();
		WZ_CORE_TRACE("Initializing glad...");
		auto _status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		WZ_CORE_ASSERT(_status, "Failed to intitialize glad");

        GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
            glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }
    void GLContext::SwapBuffers() {
        glfwSwapBuffers(m_windowHandle);
    }
    void GLContext::MakeCurrent()
    {
        if (glfwGetCurrentContext() != m_windowHandle)
        {
            glfwMakeContextCurrent(m_windowHandle);
        }
    }
}
