#pragma once

namespace Wizzy {

	enum class EventType {
		event_none = 0,
		window_close, window_resize, window_focus, window_unfocus, window_move,
		app_tick, app_update, app_render, app_frame_begin, app_frame_end, app_init, app_shutdown,
		key_press, key_release, key_repeat,
		char_sent,
		mouse_press, mouse_release, mouse_move, mouse_scroll,
		resource_register, resource_load, resource_save, resource_unload, resource_delete, resource_file_change,
		client_event,

		event_count
	};

	enum EventCategory {
		category_none = 0,
		category_application = BIT(0),
		category_input = BIT(1),
		category_keyboard = BIT(2),
		category_mouse = BIT(3),
		category_mouse_button = BIT(5),
		category_resources = BIT(4),
		category_client = BIT(6)
	};

	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const string GetName() const override { return #type; }
	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event {
	private:
		friend class EventDispatcher;
	protected:
		bool m_isHandled = false;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const string GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }

		inline bool IsHandled() const { return m_isHandled; }
	};

	class EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_event(event) {
		}

		template<typename T>
		bool Dispatch(EventFn<T> func) {
			if (m_event.GetEventType() == T::GetStaticType() && !m_event.m_isHandled) {
				m_event.m_isHandled = func(*(T*)&m_event);
				return true;
			}
			return false;
		}

		template<typename ostream>
        inline friend ostream& operator<<(ostream& os, const Event& e) {
            return os << e.ToString();
        }
	private:
		Event& m_event;
	};
}
