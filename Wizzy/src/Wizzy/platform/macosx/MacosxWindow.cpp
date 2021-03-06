#include "wzpch.h"
#include "MacosxWindow.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Wizzy/events/AppEvent.h"
#include "Wizzy/events/MouseEvent.h"
#include "Wizzy/events/CharEvent.h"
#include "Wizzy/events/KeyEvent.h"

namespace Wizzy {
    IWindow *IWindow::Create(const WindowProps& props) {
        return new MacosxWindow(props);
    }

    bool MacosxWindow::s_glfwInitialized(false);

    static void glfw_error_callback(int32 error, const char *description) {
        WZ_CORE_ERROR("GLFW ERROR ({0}): '{1}'", error, description);
    }

    MacosxWindow::MacosxWindow(const WindowProps& props) {
        Init(props);
    }

    MacosxWindow::~MacosxWindow(){
        Shutdown();
    }

    void MacosxWindow::Init(const WindowProps& props) {
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

        m_glfwWindow = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_glfwWindow);

		WZ_CORE_TRACE("Initializing glad...");
		auto _status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		WZ_CORE_ASSERT(_status, "Failed to intitialize glad");

        glfwSetWindowUserPointer(m_glfwWindow, &m_data);

        SetVSync(false);
        SetClearColor(.1f, .2f, .5f, 1.f);

		GL_CALL(glEnable(GL_BLEND));
		GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		GL_CALL(glEnable(GL_CULL_FACE));
		GL_CALL(glCullFace(GL_BACK));

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

        WZ_CORE_INFO("Welcome to Wizzy for Macosx!");
        WZ_CORE_INFO("Version: {0}", glGetString(GL_VERSION));
    }

    void MacosxWindow::Shutdown() {
        glfwDestroyWindow(m_glfwWindow);
    }

    void MacosxWindow::OnFrameBegin() {
		glClear(GL_COLOR_BUFFER_BIT);
        
    }

    void MacosxWindow::OnFrameEnd() {
		glfwPollEvents();
		glfwSwapBuffers(m_glfwWindow);
    }

    void MacosxWindow::SetVSync(bool enabled) {
        glfwSwapInterval(enabled);
        m_data.vsync = enabled;
    }

    bool MacosxWindow::IsVsync() const {
        return m_data.vsync;
    }

    void MacosxWindow::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void MacosxWindow::SetWidth(u32 width){
        glfwSetWindowSize(m_glfwWindow, width, m_data.height);
    }

    void MacosxWindow::SetHeight(u32 height){
        glfwSetWindowSize(m_glfwWindow, m_data.width, height);
    }
}