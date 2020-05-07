#include "wzpch.h"

#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Utils.h"
#include "Wizzy/Application.h"


#define __CHECK_HANDLE(handle, loaded)\
{\
	if (!IsValid(handle))\
	{\
		WZ_THROW(ResourceInvalidHandleException, handle.id); \
	}\
	\
	auto& resourceInfo = s_resourceInfo[handle]; \
	\
	if (IsLoaded(handle) != loaded)\
	{\
		if (loaded) \
		{\
			WZ_THROW(ResourceUnloadedException, resourceInfo.resPath, handle.id, resourceInfo.name()); \
		}\
		else\
		{\
			WZ_THROW(ResourceLoadedException, resourceInfo.resPath, handle.id, resourceInfo.name()); \
		}\
		\
	}\
}\

namespace Wizzy
{
	std::vector<Resource*> ResourceManagement::s_resource;
	Resource::HandleMap<ResourceInfo> ResourceManagement::s_resourceInfo;
	std::set<Resource::Handle> ResourceManagement::s_handles;
	string ResourceManagement::s_resourceDir = "";
	u32 ResourceManagement::s_freeIndicesCount = 0;
	uId ResourceManagement::s_idCounter;
	std::queue<ResourceManagement::ResourceAction> ResourceManagement::s_deferredActions;

	void ResourceManagement::Load(const Resource::Handle& handle)
	{
		WZ_CORE_TRACE("Queueing resouurce load for id {0}...", handle.id);
		
		__CHECK_HANDLE(handle, false);
		auto& info = s_resourceInfo[handle];
		WZ_CORE_TRACE("Loading resource '{0}' ({1})", info.resPath, handle.id);

		WZ_CORE_ASSERT(info.resourceIndex < s_resource.size(), "Resource index " + std::to_string(info.resourceIndex) + "' out of range");

		Resource* pResource = NULL;

		WZ_CORE_ASSERT(pResource == nullptr, "Resource flagged as unloaded, but has memory allocated");

		try
		{
			pResource = info.createFn(info.source, info.metaData);
			s_resource[info.resourceIndex] = pResource;
			if (!info.runtime) 	{WZ_CORE_INFO("Resource '{0}' loaded", info.resPath);}
			else				{WZ_CORE_TRACE("Resource '{0}' loaded", info.resPath);}
			DispatchEvent(ResourceLoadedEvent(handle));
		}
		catch (const Exception & e)
		{
			if (pResource) delete pResource;
			WZ_CORE_ERROR("Error when creating resource for handle id {0}:\n{1}", handle.id, e.GetMessage());
			return;
		}

	}

	void ResourceManagement::Save(const Resource::Handle& handle)
	{
		WZ_CORE_TRACE("Queuing save task for resource id {0}...", handle.id);
		
		if (!IsValid(handle))
		{
			WZ_THROW(ResourceInvalidHandleException, handle.id);
		}

		auto& info = s_resourceInfo[handle];
		string file = s_resourceDir + info.resPath;

		WZ_CORE_TRACE("Saving resource {0} ({1})", info.name(), handle.id);

		if (IsLoaded(handle))
		{
			WZ_CORE_TRACE("Serializing resource '{0}'", info.name());
			Resource* resource = s_resource.at(info.resourceIndex);
			ResData serialized;
			try
			{
				serialized = resource->Serialize();
			}
			catch (const Exception & e)
			{
				WZ_CORE_ERROR("Error when saving resource '{0}':\n{1}", info.name(), e.GetUnhandledMessage());
				return;
			}

			if (!info.runtime) 	{WZ_CORE_INFO("Resource '{0}' serialized", info.name());}
			else 				{WZ_CORE_TRACE("Resource '{0}' serialized", info.name());}

			info.source = serialized;

			if (!ulib::File::write(file, serialized, info.binary))
			{
				WZ_THROW(ResourceFileAccessException, file);
			}
		}

		WriteMeta(handle);

		if (!info.runtime) 	{WZ_CORE_INFO("Saved resource '{0}'", info.name());}
		else            	{WZ_CORE_TRACE("Saved resource '{0}'", info.name());}
		DispatchEvent(ResourceSavedEvent(handle));
		
	}

	void ResourceManagement::Unload(const Resource::Handle& handle)
	{
		WZ_CORE_TRACE("Queuing unload task for resource id {0}...", handle.id);
		
		__CHECK_HANDLE(handle, true);

		auto& info = s_resourceInfo[handle];
		info.isUnloading = true;

		DeferAction([handle]() 
		{
			auto& info = s_resourceInfo[handle];

			WZ_CORE_TRACE("Unloading resource (path: {0}, handle: {1})", info.resPath, info.id);

			delete s_resource[info.resourceIndex];
			s_resource[info.resourceIndex] = nullptr;

			if (!info.runtime) WZ_CORE_INFO("Unloaded resource '{0}'", info.resPath);

			info.isUnloading = false;

			DispatchEvent(ResourceUnloadedEvent(handle));
		});
		
	}

	void ResourceManagement::Reload(const Resource::Handle& handle, const PropertyTable& props)
	{
			__CHECK_HANDLE(handle, true);

			auto& resourceInfo = s_resourceInfo[handle];
			resourceInfo.metaData = props;

			Reload(handle);
	}

	void ResourceManagement::Reload(const Resource::Handle& handle)
	{
			__CHECK_HANDLE(handle, true);

			auto& info = s_resourceInfo[handle];

			WZ_CORE_TRACE("Reloading resource {0}", info.name());

			WZ_CORE_TRACE("Updating source");
			ResData currentSource;
			if (!ulib::File::read(s_resourceDir + info.resPath, &currentSource, info.binary))
			{
				WZ_THROW(ResourceFileAccessException, info.resPath);
			}

			info.source = currentSource;

			WZ_CORE_TRACE("About to unload...");
			Unload(handle);
			WZ_CORE_TRACE("About to load...");
			Load(handle);
	}

	void ResourceManagement::Delete(const Resource::Handle& handle)
	{
			auto& info = s_resourceInfo[handle];
			WZ_CORE_TRACE("Deleting resource (path: {0}, handle: {1})", info.resPath, handle.id);

			if (!IsValid(handle))
			{
				WZ_THROW(ResourceInvalidHandleException, handle.id);
			}

			if (IsLoaded(handle))
			{
				Unload(handle);
			}

			info.isDeleting = true;

			DeferAction([handle, info]()
			{
				DispatchEvent(ResourceDeletedEvent(handle));

				delete s_resource[info.resourceIndex];
				s_resource[info.resourceIndex] = nullptr;
				s_freeIndicesCount++;

				s_handles.erase(handle);
				string resPath = s_resourceInfo[handle].resPath;
				s_resourceInfo.erase(handle);

				if (!info.runtime) 	{WZ_CORE_INFO("Deleted resource '{0}'", resPath);}
				else				{WZ_CORE_TRACE("Deleted resource '{0}'", resPath);}
			});
	}

	void ResourceManagement::SetResourceDir(const string& dir)
	{
			if (!ulib::Directory::exists(dir) && dir != "")
			{
				WZ_THROW(ResourceInvalidPathException, dir);
			}
			s_resourceDir = dir;
			if (s_resourceDir.size() > 0 && (s_resourceDir[s_resourceDir.size() - 1] != '/' && s_resourceDir[s_resourceDir.size() - 1] != '\\'))  s_resourceDir += "/";

			string idCountFile = s_resourceDir + ".id";
			if (ulib::File::exists(idCountFile))
			{
				string content;
				if (ulib::File::read(idCountFile, &content))
				{
					s_idCounter = std::stoi(content);
				}
			}
	}

	void ResourceManagement::Validate(bool checkSources)
	{
			for (auto handle : s_handles)
			{
				if (s_resourceInfo.find(handle) == s_resourceInfo.end())
				{
					WZ_THROW(ResourceInvalidHandleException, handle);
				}
			}

			for (auto& [handle, info] : s_resourceInfo)
			{
				if (info.runtime)
				{
					WZ_CORE_ASSERT(IsLoaded(handle), "A runtime resource is unloaded");
					continue;
				}
				string fullPath = s_resourceDir + info.resPath;

				if (s_handles.find(handle) == s_handles.end())
				{
					WZ_THROW(ResourceInvalidHandleException, handle.id);
				}

				if (info.resourceIndex >= s_resource.size())
				{
					WZ_THROW(ResourceIndexOutOfRangeException, info.resPath, handle.id, info.name());
				}

				if (!ulib::File::exists(fullPath))
				{
					WZ_THROW(ResourceFileMissingException, info.resPath, handle.id, info.name());
				}

				if (!HasMeta(handle))
				{
					WriteMeta(handle);
				}

				if (IsLoaded(handle))
				{
					auto lastWriteTime = std::filesystem::last_write_time(s_resourceDir + info.resPath);

					if (lastWriteTime > info.lastWrite)
					{
						info.lastWrite = lastWriteTime;
						WZ_CORE_TRACE("Changed detectedfor file {0}", info.resPath);

						DispatchEvent(ResourceFileChangedEvent(handle));
					}
				}
			}
	}

	const ResourceInfo& ResourceManagement::GetInfoFor(const Resource::Handle& handle)
	{
		if (IsValid(handle))
		{
			return s_resourceInfo[handle];
		}
		else
		{
			WZ_THROW(ResourceInvalidHandleException, handle.id);
		}
	}

	PropertyTable& ResourceManagement::GetMetaData(const Resource::Handle& handle)
	{
		WZ_CORE_ASSERT(IsValid(handle), "Cannot retrieve meta data on in valid handle");

		return s_resourceInfo[handle].metaData;
	}

	Resource::Handle ResourceManagement::IdToHandle(uId id)
	{
		auto it = s_handles.find(Resource::Handle(id));
		return it != s_handles.end() ? *it : WZ_NULL_RESOURCE_HANDLE;
	}

	string ResourceManagement::UniquenizePathName(const string& path)
	{
		if (!ulib::File::exists(s_resourceDir + path)) return path;
		string p = path;
		int suffix = 1;

		while (ulib::File::exists(s_resourceDir + ulib::File::without_extension(p) + "(" + std::to_string(suffix) + ")" + ulib::File::extension_of(p)))
		{
			suffix++;
		}

		p = ulib::File::without_extension(p) + "(" + std::to_string(suffix) + ")" + ulib::File::extension_of(p);

		return p;
	}

	void ResourceManagement::DispatchEvent(ResourceEvent& e)
	{
		Application::Get().DispatchEvent(e);
	}

	void ResourceManagement::WriteMeta(const Resource::Handle& hnd)
	{
		WZ_CORE_ASSERT(IsValid(hnd), "Cannot write meta for invalid resource handle");
		const auto& info = GetInfoFor(hnd);
		WZ_CORE_ASSERT(!info.runtime, "Runtime resources CANNOT and WILL NOT have meta file associated with them");
	
		string metaFile = s_resourceDir + info.resPath + ".wz";
		string metaDataStr = info.metaData.Serialize();

		if (!ulib::File::write(metaFile, metaDataStr))
		{
			WZ_THROW(ResourceFileAccessException, metaFile);
		}
	}

	PropertyTable ResourceManagement::ReadMeta(const Resource::Handle& hnd)
	{
		WZ_CORE_ASSERT(IsValid(hnd), "Cannot get meta for invalid resource handle");
		const auto& info = GetInfoFor(hnd);
		WZ_CORE_ASSERT(!info.runtime, "Runtime resources CANNOT and WILL NOT have meta file associated with them");
		return ReadMeta(info.resPath);
	}

	PropertyTable ResourceManagement::ReadMeta(const string& resPath)
	{
		string metaFile = s_resourceDir + resPath + ".wz";
		string metaDataStr;
		if (!ulib::File::read(metaFile, &metaDataStr))
		{
			WZ_THROW(ResourceFileAccessException, metaFile);
		}
		PropertyTable metaData;
		metaData.Deserialize(metaDataStr);
		return metaData;
	}

	bool ResourceManagement::HasMeta(const Resource::Handle& hnd)
	{
		WZ_CORE_ASSERT(IsValid(hnd), "Cannot check meta for invalid resource handle");
		const auto& info = GetInfoFor(hnd);
		WZ_CORE_ASSERT(!info.runtime, "Runtime resources CANNOT and WILL NOT have meta file associated with them");
		return HasMeta(info.resPath);
	}

	bool ResourceManagement::HasMeta(const string& resPath)
	{
		return ulib::File::exists(s_resourceDir + resPath + ".wz");
	}

	bool ResourceManagement::IsValid(const Resource::Handle& handle)
	{
		return s_handles.find(handle) != s_handles.end() 
			&& s_resourceInfo.find(handle) != s_resourceInfo.end()
			&& !s_resourceInfo[handle].isDeleting;
	}

	bool ResourceManagement::IsValid(const string& resPath)
	{
		Resource::Handle hndMatch = WZ_NULL_RESOURCE_HANDLE;
		for (const auto& handle : s_handles)
		{
			auto& info = s_resourceInfo[handle];

			if (info.resPath == resPath)
			{
				hndMatch = handle;
				break;
			}
		}

		return IsValid(hndMatch);
	}

	bool ResourceManagement::IsLoaded(const Resource::Handle& handle)
	{
		return IsValid(handle) 
				&& !GetInfoFor(handle).isUnloading 
				&& s_resource[s_resourceInfo[handle].resourceIndex] != nullptr;
	}

	bool ResourceManagement::IsFileRegistered(const string& resFile)
	{
		for (auto& [h, info] : s_resourceInfo)
		{
			if (info.resPath == resFile) return true;
		}
		return false;
	}

	Resource::Handle ResourceManagement::HandleOf(const string& str)
	{
		if (ulib::File::exists(s_resourceDir + str))
		{
			Resource::Handle hndMatch = WZ_NULL_RESOURCE_HANDLE;
			for (const auto& handle : s_handles)
			{
				auto& info = s_resourceInfo[handle];

				if (info.resPath == str)
				{
					hndMatch = handle;
					break;
				}
			}

			return hndMatch;
		}
		else
		{
			Resource::Handle hndMatch = WZ_NULL_RESOURCE_HANDLE;
			for (const auto& handle : s_handles)
			{
				auto& info = s_resourceInfo[handle];

				if (info.name() == str)
				{
					hndMatch = handle;
					break;
				}
			}

			return hndMatch;
		}
	}

	string ResourceManagement::NameOf(const Resource::Handle& hnd)
	{
		if (IsValid(hnd)) return GetInfoFor(hnd).name();
		else return "N/A";
	}

	void ResourceManagement::ForEach(std::function<bool(const Resource::Handle&)> fn)
	{
		for (const auto& hnd : s_handles)
		{
			if (!fn(hnd)) break;
		}
	}

	void ResourceManagement::_Update()
	{
		while (s_deferredActions.size() > 0)
		{
			auto& action = s_deferredActions.front();

			action();

			s_deferredActions.pop();
		}
	}

	void ResourceManagement::DeferAction(ResourceAction action)
	{
		s_deferredActions.push(action);
	}

}