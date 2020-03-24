#include "wzpch.h"
#include "WindowsWindow.h"

#include <GLFW/glfw3.h>

#include "Wizzy/Events/AppEvent.h"
#include "Wizzy/Events/MouseEvent.h"
#include "Wizzy/Events/CharEvent.h"
#include "Wizzy/Events/KeyEvent.h"
#include "Wizzy/platform/OpenGL/GLContext.h"
#include "Wizzy/Renderer/RendererAPI.h"
#include "Wizzy/Renderer/API.h"

#include "Wizzy/Instrumentor.h"

namespace Wizzy {
	IWindow *IWindow::Create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

	bool WindowsWindow::s_glfwInitialized(false);

	static void glfw_error_callback(int32 error, const char *description) {
		WZ_CORE_ERROR("GLFW ERROR ({0}): '{1}'", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		Init(props);
	}

	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props) {
		WZ_PROFILE_FUNCTION();
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

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		m_glfwWindow = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);

		glfwGetWindowPos(m_glfwWindow, &m_data.posx, &m_data.posy);

		m_context = new GLContext(m_glfwWindow);

		m_context->Init();

		glfwSetWindowUserPointer(m_glfwWindow, &m_data);

		SetVSync(false);

		glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow *w, int32 width, int32 height) {

			auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));
			_data.width = width;
			_data.height = height;

			WindowResizeEvent _event(width, height);
			_data.eventCallbackFn(_event);
			});

		glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow *w) {

			auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

			WindowCloseEvent _event;
			_data.eventCallbackFn(_event);
			});

		glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow *w, int32 key, int32 scancode, int32 action, int32 mods) {

			auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

			switch (action) {
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

		glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow *w, int32 button, int32 action, int32 mods) {

			auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

			switch (action) {
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

		glfwSetScrollCallback(m_glfwWindow, [](GLFWwindow *w, double x, double y) {

			auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

			MouseScrolledEvent _event(x, y);
			_data.eventCallbackFn(_event);
			});

		glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow *w, double x, double y) {

			auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

			MouseMovedEvent _event(x, y);
			_data.eventCallbackFn(_event);
			});

		glfwSetWindowPosCallback(m_glfwWindow, [](GLFWwindow* w, int32 x, int32 y)
		{
			auto& _data = *((WindowData*)glfwGetWindowUserPointer(w));

			_data.posx = x;
			_data.posy = y;
		});

		WZ_CORE_INFO("Welcome to Wizzy for Windows!");
		WZ_CORE_INFO("API version: {0}", APIVersion());
		WZ_CORE_INFO("API vendor: {0}", APIVendor());
	}

	void WindowsWindow::Shutdown() {
		WZ_PROFILE_FUNCTION();
		glfwDestroyWindow(m_glfwWindow);
		glfwTerminate();
	}

	void WindowsWindow::OnFrameBegin() {
		WZ_PROFILE_FUNCTION();
	}

	void WindowsWindow::OnFrameEnd() {
		WZ_PROFILE_FUNCTION();
		{
			WZ_PROFILE_SCOPE("Set delta");
			double _now = glfwGetTime();
			float _delta = static_cast<float>(_now - m_lastTime);
			m_data.deltaTime = _delta;
			m_lastTime = _now;
		}

		{
			WZ_PROFILE_SCOPE("Poll events");
			glfwPollEvents();
		}
		{
			WZ_PROFILE_SCOPE("Swap buffers");
			m_context->SwapBuffers();
		}
	}

	void WindowsWindow::SetVSync(bool enabled) {
		WZ_PROFILE_FUNCTION();
		glfwSwapInterval(enabled);
		m_data.vsync = enabled;
	}

	bool WindowsWindow::IsVsync() const {
		return m_data.vsync;
	}

	GraphicsContext& WindowsWindow::GetContext()
	{
		return *m_context;
	}

	void WindowsWindow::SetWidth(u32 width) {
		glfwSetWindowSize(m_glfwWindow, width, m_data.height);
	}

	void WindowsWindow::SetHeight(u32 height) {
		glfwSetWindowSize(m_glfwWindow, m_data.width, height);
	}
}
