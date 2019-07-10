#pragma once

#include "Event.h"
#include "../Core.h"

namespace Wizzy {

	class  MouseMovedEvent
		: public Event {
	private:
		double m_x, m_y;
	public:

		EVENT_CLASS_TYPE(mouse_move)
		EVENT_CLASS_CATEGORY(category_input | category_mouse)

		inline MouseMovedEvent(double x, double y) {
			m_x = x;
			m_y = y;
		}

		inline const double& GetX() const { return m_x; }
		inline const double& GetY() const { return m_y; }

		inline virtual string ToString() const override {
			return GetName() + ": X'" + std::to_string(m_x) + "'" + ": Y'" + std::to_string(m_y) + "'";
		}
	};

	class  MousePressedEvent
		: public Event {
	private:
		int m_buttonCode;
	public:

		EVENT_CLASS_TYPE(mouse_press)
		EVENT_CLASS_CATEGORY(category_input | category_mouse_button)

		inline MousePressedEvent(int buttonCode) {
			m_buttonCode = buttonCode;
		}

		inline const int& GetButtonCode() const { return m_buttonCode; }

		inline virtual string ToString() const override {
			return GetName() + ": '" + std::to_string(m_buttonCode) + "'";
		}
	};

	class  MouseReleasedEvent
		: public Event {
	private:
		int m_buttonCode;
	public:

		EVENT_CLASS_TYPE(mouse_release)
		EVENT_CLASS_CATEGORY(category_input | category_mouse_button)

		inline MouseReleasedEvent(int buttonCode){
			m_buttonCode = buttonCode;
		}

		inline const int& GetButtonCode() const { return m_buttonCode; }

		inline virtual string ToString() const override {
			return GetName() + ": '" + std::to_string(m_buttonCode) + "'";
		}
	};

	class  MouseScrolledEvent
		: public Event {
	private:
		double m_xOffset, m_yOffset;
	public:

		EVENT_CLASS_TYPE(mouse_scroll)
		EVENT_CLASS_CATEGORY(category_input | category_mouse_button)

		inline MouseScrolledEvent(double xOffset, double yOffset) {
			m_xOffset = xOffset;
			m_yOffset = yOffset;
		}

		inline const double& GetXOffset() const { return m_xOffset; }
		inline const double& GetYOffset() const { return m_yOffset; }

		inline virtual string ToString() const override {
			return GetName() + ": X'" + std::to_string(m_xOffset) + "'" + ": Y'" + std::to_string(m_yOffset) + "'";
		}
	};
}
