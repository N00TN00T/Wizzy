#pragma once

#include "Wizzy/Events/Event.h"

namespace Wizzy {

    class ResourceAddEvent
		: public Event {
	private:
		ResourceHandle m_handle;
        string m_file;
	public:
		inline ResourceAddEvent(const ResourceHandle& handle, const string& file)
            : m_handle(handle), m_file(file) { }

		inline const ResourceHandle& GetHandle() const { return m_handle; }
		inline const string& GetFile() const { return m_file; }

		inline virtual string ToString() const override {
			return GetName() + ": Handle'" + string(m_handle) + "' : File'" + string(m_file) + "'";
		}

		EVENT_CLASS_TYPE(resource_add)
		EVENT_CLASS_CATEGORY(category_resources)
	};

    class ResourceSaveEvent
		: public Event {
	private:
		ResourceHandle m_handle;
        string m_file;
	public:
		inline ResourceSaveEvent(const ResourceHandle& handle, const string& file)
            : m_handle(handle), m_file(file) { }

		inline const ResourceHandle& GetHandle() const { return m_handle; }
		inline const string& GetFile() const { return m_file; }

		inline virtual string ToString() const override {
			return GetName() + ": Handle'" + string(m_handle) + "' : File'" + string(m_file) + "'";
		}

		EVENT_CLASS_TYPE(resource_save)
		EVENT_CLASS_CATEGORY(category_resources)
	};

    class ResourceDeleteEvent
		: public Event {
	private:
		ResourceHandle m_handle;
        string m_file;
	public:
		inline ResourceDeleteEvent(const ResourceHandle& handle, const string& file)
            : m_handle(handle), m_file(file) { }

		inline const ResourceHandle& GetHandle() const { return m_handle; }
		inline const string& GetFile() const { return m_file; }

		inline virtual string ToString() const override {
			return GetName() + ": Handle'" + string(m_handle) + "' : File'" + string(m_file) + "'";
		}

		EVENT_CLASS_TYPE(resource_delete)
		EVENT_CLASS_CATEGORY(category_resources)
	};

    class ResourceRenameEvent
		: public Event {
	private:
        ResourceHandle m_newHandle;
		ResourceHandle m_oldHandle;
        string m_file;
	public:
		inline ResourceRenameEvent(const ResourceHandle& oldHandle,
                                   const ResourceHandle& newHandle,
                                   const string& file)
            : m_newHandle(newHandle), m_oldHandle(oldHandle), m_file(file) { }

		inline const ResourceHandle& GetOldHandle() const { return m_oldHandle; }
        inline const ResourceHandle& GetNewHandle() const { return m_newHandle; }
		inline const string& GetFile() const { return m_file; }

		inline virtual string ToString() const override {
			return GetName() + ": OldHandle'" + string(m_oldHandle) + "': OldHandle'" + string(m_oldHandle) + "' : File'" + string(m_file) + "'";
		}

		EVENT_CLASS_TYPE(resource_rename)
		EVENT_CLASS_CATEGORY(category_resources)
	};
}
