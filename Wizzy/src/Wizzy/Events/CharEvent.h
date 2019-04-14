#pragma once

#include "Event.h"
#include "../Core.h"

namespace Wizzy {
	class WZ_API CharEvent
		: public Event {
	private:
		uint8 m_char;
	public:
		inline CharEvent(uint8 c) {
			m_char = c;
		}

		EVENT_CLASS_TYPE(char_sent)
		EVENT_CLASS_CATEGORY(category_input | category_keyboard)

		inline virtual string ToString() const override {
			return GetName() + ": '" + std::string(1, m_char) + "'";
		}

		inline const uint8& GetChar() { return m_char; }
	};
}