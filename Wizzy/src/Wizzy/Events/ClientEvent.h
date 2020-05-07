#pragma once

#include "Wizzy/Events/Event.h"
#include "Wizzy/PropertyTable.h"

namespace Wizzy
{
	class ClientEvent
		: public Event
	{
	public:
		inline ClientEvent(int16 eventId, const PropertyTable& eventData = PropertyTable()) : m_eventId(eventId), m_eventData(eventData) {}

		EVENT_CLASS_TYPE(client_event);
		EVENT_CLASS_CATEGORY(category_client);

		inline int16 GetEventId() const { return m_eventId; }
		inline const PropertyTable& GetEventData() const { return m_eventData; }

	private:
		int16 m_eventId;
		PropertyTable m_eventData;
	};
}