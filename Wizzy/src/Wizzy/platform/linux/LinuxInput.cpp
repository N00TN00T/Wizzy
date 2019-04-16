#include <wzpch.h>
#include "LinuxInput.h"

#include "Wizzy/Application.h"
#include <GLFW/glfw3.h>

namespace Wizzy {
    Input *Input::s_instance = new LinuxInput();

    bool LinuxInput::GetKeyImpl(KeyCode keycode) {

        auto& _app = Application::Get();

        auto _window = static_cast<GLFWwindow*>(_app.GetWindow().GetNativeWindow());
        auto _state = glfwGetKey(_window, keycode);
        return _state == GLFW_PRESS || _state == GLFW_REPEAT;

    }

    bool LinuxInput::GetMouseButtonImpl(MouseCode mousecode) {
        auto& _app = Application::Get();

        auto _window = static_cast<GLFWwindow*>(_app.GetWindow().GetNativeWindow());
        auto _state = glfwGetMouseButton(_window, mousecode);
        return _state == GLFW_PRESS;
    }

    vec2 LinuxInput::GetMousePosImpl() {
		auto& _app = Application::Get();
		auto _window = static_cast<GLFWwindow*>(_app.GetWindow().GetNativeWindow());

		double _x, _y;
		glfwGetCursorPos(_window, &_x, &_y);

		return vec2(_x, _y);
	}
}