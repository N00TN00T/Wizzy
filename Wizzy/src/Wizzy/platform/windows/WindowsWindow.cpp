#include "wzpch.h"
#include "WindowsWindow.h"

#include <GLFW/glfw3.h>
#include <gl/GL.h>

namespace Wizzy {
    IWindow *IWindow::Create(const WindowProps& props) {
        return new WindowsWindow(props);
    }

    bool WindowsWindow::s_glfwInitialized(false);

    WindowsWindow::WindowsWindow(const WindowProps& props) {
        Init(props);
    }

    WindowsWindow::~WindowsWindow(){
        Shutdown();
    }

    void WindowsWindow::Init(const WindowProps& props) {
        m_data.title = props.title;
        m_data.width = props.width;
        m_data.height = props.height;

        if (!s_glfwInitialized) {
            
            WZ_CORE_TRACE("Initializing glfw...");

            auto _result = glfwInit();

            WZ_CORE_ASSERT(_result != GLFW_NO_ERROR, "Glfw init error code: '{0}'", _result);
        
            s_glfwInitialized = true;
        }

        WZ_CORE_TRACE("Creating window '{0}'...", props.title);

        m_glfwWindow = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_glfwWindow);
        glfwSetWindowUserPointer(m_glfwWindow, &m_data);

        SetVSync(false);
        SetClearColor(.1f, .2f, .5f, 1.f);
    }

    void WindowsWindow::Shutdown() {
        glfwDestroyWindow(m_glfwWindow);
    }

    void WindowsWindow::OnFrameBegin() {
        glfwPollEvents();
        glfwSwapBuffers(m_glfwWindow);
    }

    void WindowsWindow::OnFrameEnd() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void WindowsWindow::SetVSync(bool enabled) {
        glfwSwapInterval(enabled);
        m_data.vsync = enabled;
    }

    bool WindowsWindow::IsVsync() const {
        return m_data.vsync;
    }

    void WindowsWindow::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void WindowsWindow::SetWidth(u32 width){
        glfwSetWindowSize(m_glfwWindow, width, m_data.height);
    }

    void WindowsWindow::SetHeight(u32 height){
        glfwSetWindowSize(m_glfwWindow, m_data.width, height);
    }
}