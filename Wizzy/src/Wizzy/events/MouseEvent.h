#pragma once

#include "event.h"
#include "../Core.h"

namespace Wizzy {

	class WZ_API MouseMovedEvent
		: public Event {
	private:
		float m_x, m_y;
	public:

		EVENT_CLASS_TYPE(mouse_move)
		EVENT_CLASS_CATEGORY(category_input | category_mouse)

		inline MouseMovedEvent(float x, float y) {
			m_x = x;
			m_y = y;
		}

		inline const float& GetX() const { return m_x; }
		inline const float& GetY() const { return m_y; }

		inline virtual string ToString() const override {
			return GetName() + ": X'" + std::to_string(m_x) + "'" + ": Y'" + std::to_string(m_y) + "'";
		}
	};

	class WZ_API MousePressedEvent
		: public Event {
	private:
		int m_buttonCode;
	public:

		EVENT_CLASS_TYPE(mouse_press)
		EVENT_CLASS_CATEGORY(category_input | category_mouse)

		inline MousePressedEvent(int buttonCode) {
			m_buttonCode = buttonCode;
		}

		inline const int& GetButtonCode() const { return m_buttonCode; }

		inline virtual string ToString() const override {
			return GetName() + ": '" + std::to_string(m_buttonCode) + "'";
		}
	};

	class WZ_API MouseReleasedEvent
		: public Event {
	private:
		int m_buttonCode;
	public:

		EVENT_CLASS_TYPE(mouse_release)
		EVENT_CLASS_CATEGORY(category_input | category_mouse)

		inline MouseReleasedEvent(int buttonCode){
			m_buttonCode = buttonCode;
		}

		inline const int& GetButtonCode() const { return m_buttonCode; }

		inline virtual string ToString() const override {
			return GetName() + ": '" + std::to_string(m_buttonCode) + "'";
		}
	};

	class WZ_API MouseScrolledEvent
		: public Event {
	private:
		float m_xOffset, m_yOffset;
	public:

		EVENT_CLASS_TYPE(mouse_scroll)
		EVENT_CLASS_CATEGORY(category_input | category_mouse)

		inline MouseScrolledEvent(float xOffset, float yOffset) {
			m_xOffset = xOffset;
			m_yOffset = yOffset;
		}

		inline const float& GetXOffset() const { return m_xOffset; }
		inline const float& GetYOffset() const { return m_yOffset; }

		inline virtual string ToString() const override {
			return GetName() + ": X'" + std::to_string(m_xOffset) + "'" + ": Y'" + std::to_string(m_yOffset) + "'";
		}
	};
}
