#pragma once

#include "Wizzy/Events/Event.h"
#include "Wizzy/Resource/Resource.h"

namespace Wizzy 
{

	class ResourceEvent : public Event
	{
	public:
		inline const Resource::Handle& GetResourceHandle() const { return m_hResource; }

	protected:
		inline ResourceEvent(Resource::Handle hResource)
		{
			m_hResource = hResource;
		}

		Resource::Handle m_hResource;
	};

	class ResourceRegisteredEvent : public ResourceEvent
	{
	public:
		inline ResourceRegisteredEvent(Resource::Handle hResource) : ResourceEvent(hResource) {}

		EVENT_CLASS_TYPE(resource_register);
		EVENT_CLASS_CATEGORY(category_resources);

		inline virtual string ToString() const override
		{
			return GetName() + ": '" + std::to_string(m_hResource.id) + "'";
		}
	};

	class ResourceLoadedEvent : public ResourceEvent
	{
	public:
		inline ResourceLoadedEvent(Resource::Handle hResource) : ResourceEvent(hResource) {}

		EVENT_CLASS_TYPE(resource_load);
		EVENT_CLASS_CATEGORY(category_resources);

		inline virtual string ToString() const override
		{
			return GetName() + ": '" + std::to_string(m_hResource.id) + "'";
		}
	};

	class ResourceSavedEvent : public ResourceEvent
	{
	public:
		inline ResourceSavedEvent(Resource::Handle hResource) : ResourceEvent(hResource) {}

		EVENT_CLASS_TYPE(resource_save);
		EVENT_CLASS_CATEGORY(category_resources);

		inline virtual string ToString() const override
		{
			return GetName() + ": '" + std::to_string(m_hResource.id) + "'";
		}
	};

	class ResourceUnloadedEvent : public ResourceEvent
	{
	public:
		inline ResourceUnloadedEvent(Resource::Handle hResource) : ResourceEvent(hResource) {}

		EVENT_CLASS_TYPE(resource_unload);
		EVENT_CLASS_CATEGORY(category_resources);

		inline virtual string ToString() const override
		{
			return GetName() + ": '" + std::to_string(m_hResource.id) + "'";
		}
	};

	class ResourceDeletedEvent : public ResourceEvent
	{
	public:
		inline ResourceDeletedEvent(Resource::Handle hResource) : ResourceEvent(hResource) {}

		EVENT_CLASS_TYPE(resource_delete);
		EVENT_CLASS_CATEGORY(category_resources);

		inline virtual string ToString() const override
		{
			return GetName() + ": '" + std::to_string(m_hResource.id) + "'";
		}
	};

	class ResourceFileChangedEvent : public ResourceEvent
	{
	public:
		inline ResourceFileChangedEvent(Resource::Handle hResource) : ResourceEvent(hResource) {}

		EVENT_CLASS_TYPE(resource_file_change);
		EVENT_CLASS_CATEGORY(category_resources);

		inline virtual string ToString() const override
		{
			return GetName() + ": '" + std::to_string(m_hResource.id) + "'";
		}
	};

}
