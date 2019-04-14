#pragma once
#include "Event.h"
#include "../Core.h"

namespace Wizzy {
	class WZ_API KeyEvent
		: public Event {
	protected:
		int m_keyCode;
		inline KeyEvent(int keyCode) {
			m_keyCode = keyCode;
		}

	public:
		inline const int& GetKeyCode() const { return m_keyCode; }
	};

	class WZ_API KeyPressedEvent
		: public KeyEvent {
	public:
		inline KeyPressedEvent(int keycode) : KeyEvent(keycode) {}

		EVENT_CLASS_TYPE(key_press)
		EVENT_CLASS_CATEGORY(category_input | category_keyboard)

		inline virtual string ToString() const override {
			return GetName() + ": '" + std::to_string(m_keyCode) + "'";
		}
	};

	class WZ_API KeyReleasedEvent
		: public KeyEvent {
	public:
		inline KeyReleasedEvent(int keycode) : KeyEvent(keycode) { }

		EVENT_CLASS_TYPE(key_release)
		EVENT_CLASS_CATEGORY(category_input | category_keyboard)

		inline virtual string ToString() const override {
			return GetName() + ": '" + std::to_string(m_keyCode) + "'";
		}
	};

	class WZ_API KeyRepeatEvent
		: public KeyEvent {
	public:
		inline KeyRepeatEvent(int keycode) : KeyEvent(keycode) {}

		EVENT_CLASS_TYPE(key_repeat)
		EVENT_CLASS_CATEGORY(category_input | category_keyboard)

		inline virtual string ToString() const override {
			return GetName() + ": '" + std::to_string(m_keyCode) + "'";
		}
	};
}
