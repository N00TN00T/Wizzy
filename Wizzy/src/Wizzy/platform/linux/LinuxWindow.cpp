#include "wzpch.h"
#include "LinuxWindow.h"

#include <GLFW/glfw3.h>

#include "Wizzy/Events/AppEvent.h"
#include "Wizzy/Events/MouseEvent.h"
#include "Wizzy/Events/CharEvent.h"
#include "Wizzy/Events/KeyEvent.h"
#include "Wizzy/platform/OpenGL/GLContext.h"
#include "Wizzy/Renderer/RendererAPI.h"
#include "Wizzy/Renderer/API.h"

namespace Wizzy {
    IWindow *IWindow::Create(const WindowProps& props) {
        return new LinuxWindow(props);
    }

    bool LinuxWindow::s_glfwInitialized(false);

    static void glfw_error_callback(int32 error, const char *description) {
        WZ_CORE_ERROR("GLFW ERROR ({0}): '{1}'", error, description);
    }

    LinuxWindow::LinuxWindow(const WindowProps& props) {
        Init(props);
    }

    LinuxWindow::~LinuxWindow(){
        Shutdown();
    }

    void LinuxWindow::Init(const WindowProps& props) {
        m_data.title = props.title;
        m_data.width = props.width;
        m_data.height = props.height;

        if (!s_glfwInitialized) {

            WZ_CORE_TRACE("Initializing glfw...");

            auto _result = glfwInit();

            glfwSetErrorCallback(glfw_error_callback);

            WZ_CORE_ASSERT(_result != GLFW_NO_ERROR, "Error initializing glfw");

            s_glfwInitialized = true;
        }

        WZ_CORE_TRACE("Creating window '{0}'...",
                        props.title);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        m_glfwWindow = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);

        m_context = new GLContext(m_glfwWindow);

        m_context->Init();

        glfwSetWindowUserPointer(m_glfwWindow, &m_data);

        SetVSync(false);

        glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow *w, int32 width, int32 height){

            auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));
            _data.width = width;
            _data.height = height;

            WindowResizeEvent _event(width, height);
            _data.eventCallbackFn(_event);
        });

        glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow *w){

            auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

            WindowCloseEvent _event;
            _data.eventCallbackFn(_event);
        });

        glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow *w, int32 key, int32 scancode, int32 action, int32 mods){

            auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

            switch(action) {
            case GLFW_PRESS:
				{
					KeyPressedEvent _event(key);
					_data.eventCallbackFn(_event);
					break;
				}
            case GLFW_RELEASE:
				{
					KeyReleasedEvent _event(key);
					_data.eventCallbackFn(_event);
					break;
				}
            case GLFW_REPEAT:
				{
					KeyRepeatEvent _event(key);
					_data.eventCallbackFn(_event);
					break;
				}
            }


        });

        glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow *w, int32 button, int32 action, int32 mods){

            auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

            switch(action) {
            case GLFW_PRESS:
				{
					MousePressedEvent _event(button);
					_data.eventCallbackFn(_event);
					break;
				}
            case GLFW_RELEASE:
				{
					MouseReleasedEvent _event(button);
					_data.eventCallbackFn(_event);
					break;
				}
            }
        });

		glfwSetCharCallback(m_glfwWindow, [](GLFWwindow *w, uint32 c) {

			auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

			CharEvent _event(c);
			_data.eventCallbackFn(_event);
		});

        glfwSetScrollCallback(m_glfwWindow, [](GLFWwindow *w, double x, double y){

            auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

            MouseScrolledEvent _event(x, y);
            _data.eventCallbackFn(_event);
        });

        glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow *w, double x, double y){

            auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

            MouseMovedEvent _event(x, y);
            _data.eventCallbackFn(_event);
        });

        WZ_CORE_INFO("Welcome to Wizzy for Linux!");
        WZ_CORE_INFO("API version: {0}", APIVersion());
        WZ_CORE_INFO("API vendor: {0}", APIVendor());
    }

    void LinuxWindow::Shutdown() {
        glfwDestroyWindow(m_glfwWindow);
        glfwTerminate();
    }

    void LinuxWindow::OnFrameBegin() {

    }

    void LinuxWindow::OnFrameEnd() {

        double _now = glfwGetTime();
        float _delta = static_cast<float>(_now - m_lastTime);
        m_data.deltaTime = _delta;
        m_lastTime = _now;

        glfwPollEvents();
        m_context->SwapBuffers();
    }

    void LinuxWindow::SetVSync(bool enabled) {
        glfwSwapInterval(enabled);
        m_data.vsync = enabled;
    }

    bool LinuxWindow::IsVsync() const {
        return m_data.vsync;
    }

    void LinuxWindow::SetWidth(u32 width){
        glfwSetWindowSize(m_glfwWindow, width, m_data.height);
    }

    void LinuxWindow::SetHeight(u32 height){
        glfwSetWindowSize(m_glfwWindow, m_data.width, height);
    }
}
