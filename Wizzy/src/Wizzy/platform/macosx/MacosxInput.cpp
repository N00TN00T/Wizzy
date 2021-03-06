#include <wzpch.h>
#include "MacosxInput.h"

#include "Wizzy/Application.h"
#include <GLFW/glfw3.h>

namespace Wizzy {
    Input *Input::s_instance = new MacosxInput();

    bool MacosxInput::GetKeyImpl(KeyCode keycode) {

        auto& _app = Application::Get();

        auto _window = static_cast<GLFWwindow*>(_app.GetWindow().GetNativeWindow());
        auto _state = glfwGetKey(_window, keycode);
        return _state == GLFW_PRESS || _state == GLFW_REPEAT;

    }

    bool MacosxInput::GetMouseButtonImpl(MouseCode mousecode) {
        auto& _app = Application::Get();

        auto _window = static_cast<GLFWwindow*>(_app.GetWindow().GetNativeWindow());
        auto _state = glfwGetMouseButton(_window, mousecode);
        return _state == GLFW_PRESS;
    }

    vec2 MacosxInput::GetMousePosImpl() {
		auto& _app = Application::Get();
		auto _window = static_cast<GLFWwindow*>(_app.GetWindow().GetNativeWindow());

		double _x, _y;
		glfwGetCursorPos(_window, &_x, &_y);

		return vec2(_x, _y);
	}
}