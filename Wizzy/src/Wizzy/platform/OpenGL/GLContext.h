#pragma once

#include "Wizzy/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Wizzy {

    class GLContext
        : public GraphicsContext {
    public:
        GLContext(GLFWwindow *windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;
    private:
        GLFWwindow *m_windowHandle;
    };

}
