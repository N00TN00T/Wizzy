#include "wzpch.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/OpenGLContext.h"

namespace Wizzy {
    OpenGLContext::OpenGLContext(GLFWwindow *windowHandle)
        : m_windowHandle(windowHandle) {
        WZ_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::Init() {
        glfwMakeContextCurrent(m_windowHandle);
		WZ_CORE_TRACE("Initializing glad...");
		auto _status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		WZ_CORE_ASSERT(_status, "Failed to intitialize glad");

		GL_CALL(glEnable(GL_CULL_FACE));
		GL_CALL(glCullFace(GL_BACK));

        //GL_CALL(glEnable(GL_DEPTH_TEST));
        //GL_CALL(glDepthFunc(GL_GREATER));
    }
    void OpenGLContext::SwapBuffers() {
        glfwSwapBuffers(m_windowHandle);
    }

    string OpenGLContext::APIVersion() {
        return reinterpret_cast< char const* >(glGetString(GL_VERSION));
    }

    string OpenGLContext::APIVendor() {
        return reinterpret_cast< char const* >(glGetString(GL_VENDOR))
                + string(" (")
                + reinterpret_cast< char const* >(glGetString(GL_RENDERER))
                + string(")");
    }
}
