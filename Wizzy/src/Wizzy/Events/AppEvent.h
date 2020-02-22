#pragma once

#include "Event.h"
#include "../Core.h"

namespace Wizzy {

	/* ------------------------------
	 *	WINDOW EVENTS
	 * ------------------------------*/
	class  WindowCloseEvent
		: public Event {
	public:
		inline WindowCloseEvent() {}

		EVENT_CLASS_TYPE(window_close)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class  WindowResizeEvent
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

	class  WindowFocusEvent
		: public Event {
	public:
		inline WindowFocusEvent() {}

		EVENT_CLASS_TYPE(window_focus)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class  WindowUnfocusEvent
		: public Event {
	public:
		inline WindowUnfocusEvent() {}

		EVENT_CLASS_TYPE(window_unfocus)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class  WindowMoveEvent
		: public Event {
	public:
		inline WindowMoveEvent() {}

		EVENT_CLASS_TYPE(window_move)
		EVENT_CLASS_CATEGORY(category_application)
	};

	/* ------------------------------
	 *	APP EVENTS
	 * ------------------------------*/
	class AppInitEvent
 		: public Event {
 	public:
 		inline AppInitEvent() {}

 		EVENT_CLASS_TYPE(app_init)
 		EVENT_CLASS_CATEGORY(category_application)
 	};
	class  AppTickEvent
		: public Event {
	public:
		inline AppTickEvent() {}

		EVENT_CLASS_TYPE(app_tick)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class  AppUpdateEvent
		: public Event {
	public:
		inline AppUpdateEvent(const float& deltaTime)
		 	: m_deltaTime(deltaTime) {}

		EVENT_CLASS_TYPE(app_update)
		EVENT_CLASS_CATEGORY(category_application)

		inline const float& GetDeltaTime() const {
			return m_deltaTime;
		}

		inline virtual string ToString() const override {
			return GetName() + " deltaTime: " + std::to_string(m_deltaTime);
		}

	private:
		float m_deltaTime;
	};

	class  AppRenderEvent
		: public Event {
	public:
		inline AppRenderEvent() {}

		EVENT_CLASS_TYPE(app_render)
		EVENT_CLASS_CATEGORY(category_application)
	};

	class AppFrameBeginEvent
		: public Event {
	public:
		inline AppFrameBeginEvent(const float& deltaTime)
			: m_deltaTime(deltaTime) {}

		EVENT_CLASS_TYPE(app_frame_begin)
		EVENT_CLASS_CATEGORY(category_application)

		inline const float& GetDeltaTime() const {
			return m_deltaTime;
		}

		inline virtual string ToString() const override {
			return GetName() + " deltaTime: " + std::to_string(m_deltaTime);
		}

	private:
		float m_deltaTime;
	};

	class AppFrameEndEvent
		: public Event {
	public:
		inline AppFrameEndEvent(const float& deltaTime)
			: m_deltaTime(deltaTime) {}

		EVENT_CLASS_TYPE(app_frame_end)
		EVENT_CLASS_CATEGORY(category_application)

		inline const float& GetDeltaTime() const {
			return m_deltaTime;
		}

		inline virtual string ToString() const override {
			return GetName() + " deltaTime: " + std::to_string(m_deltaTime);
		}

	private:
		float m_deltaTime;
	};

	class AppShutdownEvent
		: public Event
	{
	public:
		inline AppShutdownEvent(int32 code = 0)
			: m_code(code)
		{
		}

		EVENT_CLASS_TYPE(app_shutdown)
			EVENT_CLASS_CATEGORY(category_application)

			inline const float& GetExitCode() const
		{
			return m_code;
		}

		inline virtual string ToString() const override
		{
			return GetName() + " code: " + std::to_string(m_code);
		}

	private:
		int32 m_code;
	};
}
