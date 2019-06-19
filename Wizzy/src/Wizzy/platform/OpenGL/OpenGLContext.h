#pragma once

#include "Wizzy/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Wizzy {

    class OpenGLContext
        : public GraphicsContext {
    public:
        OpenGLContext(GLFWwindow *windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;
        virtual void ClearBuffer() override;
        virtual void SetClearColor(float r, float g, float b, float a) override;
        virtual string APIVersion() override;
        virtual string APIVendor() override;
    private:
        GLFWwindow *m_windowHandle;
    };

}
