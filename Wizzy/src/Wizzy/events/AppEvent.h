#pragma once

#include "event.h"
#include "../Core.h"

namespace Wizzy {

	/* ------------------------------
	 *	WINDOW EVENTS
	 * ------------------------------*/
	class WZ_API WindowCloseEvent
		: public Event {
	public:
		inline WindowCloseEvent() {}

		EVENT_CLASS_TYPE(window_close)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class WZ_API WindowResizeEvent
		: public Event {
	private:
		u32 m_x;
		u32 m_y;
	public:
		inline WindowResizeEvent(u32 x, u32 y) {
			m_x = x;
			m_y = y;
		}

		inline const u32& GetX() const { return m_x; }
		inline const u32& GetY() const { return m_y; }

		inline virtual string ToString() const override {
			return GetName() + ": X'" + std::to_string(m_x) + "'" + ": Y'" + std::to_string(m_y) + "'";
		}

		EVENT_CLASS_TYPE(window_resize)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class WZ_API WindowFocusEvent
		: public Event {
	public:
		inline WindowFocusEvent() {}

		EVENT_CLASS_TYPE(window_focus)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class WZ_API WindowUnfocusEvent
		: public Event {
	public:
		inline WindowUnfocusEvent() {}

		EVENT_CLASS_TYPE(window_unfocus)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class WZ_API WindowMoveEvent
		: public Event {
	public:
		inline WindowMoveEvent() {}

		EVENT_CLASS_TYPE(window_move)
		EVENT_CLASS_CATEGORY(category_application)
	};

	/* ------------------------------
	 *	APP EVENTS
	 * ------------------------------*/
	class WZ_API AppTickEvent
		: public Event {
	public:
		inline AppTickEvent() {}

		EVENT_CLASS_TYPE(app_tick)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class WZ_API AppUpdateEvent
		: public Event {
	public:
		inline AppUpdateEvent() {}

		EVENT_CLASS_TYPE(app_update)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class WZ_API AppRenderEvent
		: public Event {
	public:
		inline AppRenderEvent() {}

		EVENT_CLASS_TYPE(app_render)
		EVENT_CLASS_CATEGORY(category_application)
	};
}