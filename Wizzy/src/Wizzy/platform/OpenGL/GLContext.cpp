#include "wzpch.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLContext.h"

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
