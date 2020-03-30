#include "wzpch.h"

#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Utils.h"
#include "Wizzy/Application.h"


#define __CHECK_HANDLE(handle, loaded)\
{\
	if (!IsValid(handle))\
	{\
		WZ_THROW(ResourceInvalidHandleException, handle); \
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


	/*void ResourceManagement::LoadResourceList(const string& listFile)
	{
		string fullPath = s_resourceDir + listFile;

		string data = "";
		if (!ulib::File::read(fullPath, &data, true))
		{
			WZ_THROW(ResourceFileAccessException, fullPath);
		}

		std::stringstream s(data);
		string line;
		while (std::getline(s, line))
		{
			if (line.find(';') == string::npos) WZ_THROW(ResourceParseErrorException, listFile);
			string resPath = line.substr(0, line.find_first_of(';'));
			line = line.substr(line.find_first_of(';') + 1);
			string typeStr = line.substr(0, line.find_first_of(';'));
			line = line.substr(line.find_first_of(';') + 1);
			string idStr = line;
			uId id = 0;
			try
			{
				id = std::stoi(idStr);
			}
			catch (const std::exception& e)
			{
				WZ_THROW(Exception, "Could not convert string '" + idStr + "' to int: " + string(e.what()));
			}

			try
			{
				if (typeStr == typestr(Script))
				{
					Load<Script>(resPath, id);
				}
				else if (typeStr == typestr(Shader))
				{
					Load<Shader>(resPath, id);
				}
				else if (typeStr == typestr(Material))
				{
					Load<Material>(resPath, id);
				}
				else if (typeStr == typestr(Texture))
				{
					Load<Texture>(resPath, id);
				}
				else if (typeStr == typestr(RenderTarget))
				{
					Load<RenderTarget>(resPath, id);
				}
				else
				{
					WZ_THROW(Exception, "Unhandled or invalid type string: " + typeStr);
				}
			}
			catch (const Exception& e)
			{
				WZ_CORE_ERROR("Exception when loading from resource list");
				WZ_CORE_ERROR(e.GetUnhandledMessage());
			}
		}

		WZ_CORE_INFO("Successfully loaded Resource List!");
	}

	void ResourceManagement::WriteResourceList(const string& listFile)
	{
		string list = "";

		for (auto handle : s_handles)
		{
			auto& info = s_resourceInfo[handle];

			list += info.resPath + ";" + info.type + ";" +  std::to_string(handle.id) + "\n";
		}

		if (!ulib::File::write(s_resourceDir + listFile, list, true))
		{
			WZ_THROW(ResourceFileAccessException, s_resourceDir + listFile);
		}

		for (auto& hnd : s_handles)
		{
			if (IsValid(hnd) && IsLoaded(hnd))
			{
				Save(hnd);
			}
		}
	}
	*/

	void ResourceManagement::Load(Resource::Handle handle)
	{
		__CHECK_HANDLE(handle, false);
		auto& info = s_resourceInfo[handle];
		WZ_CORE_TRACE("Loading resource '{0}'", info.resPath);
		
		WZ_CORE_ASSERT(info.resourceIndex < s_resource.size(), "Resource index " + std::to_string(info.resourceIndex) + "' out of range");

		Resource* pResource = NULL;

		WZ_CORE_ASSERT(pResource == nullptr, "Resource already loaded");

		try
		{
			info.props.SetProperty("SourceFile", s_resourceDir + info.resPath);
			pResource = info.createFn(info.source, info.props);
			s_resource[info.resourceIndex] = pResource;
			DispatchEvent(ResourceLoadedEvent(handle));
			WZ_CORE_INFO("Resource '{0}' loaded", info.resPath);
		}
		catch (const Exception& e)
		{
			if (pResource) delete pResource;
			WZ_CORE_ERROR("Error when creating resource for handle id {0}:", handle.id);
			Log::SetExtra(false);
			WZ_CORE_ERROR("    {0}", e.GetMessage());
			Log::SetExtra(true);
			return;
		}
	}

	void ResourceManagement::Save(Resource::Handle handle)
	{
		__CHECK_HANDLE(handle, true);

		auto& info = s_resourceInfo[handle];

		Resource* resource = s_resource.at(info.resourceIndex);
		WZ_CORE_TRACE("Serializing resource '{0}'", info.name());
		ResData serialized;
		try
		{
			serialized = resource->Serialize();
		}
		catch (const Exception& e)
		{
			WZ_CORE_ERROR("Error when saving resource '{0}':", info.name());
			Log::SetExtra(false);
			WZ_CORE_ERROR(e.GetUnhandledMessage());
			Log::SetExtra(true);
			return;
		}

		WZ_CORE_INFO("Resource '{0}' serialized", info.name());

		info.source = serialized;

		string file = s_resourceDir + info.resPath;
		if (!ulib::File::write(file, info.source, true))
		{
			WZ_THROW(ResourceFileAccessException, file);
		}

		try
		{
			// Take the props in the resource and apply the ones in info
			PropertyTable& resourceProps = resource->GetProps();
			resourceProps.Deserialize(info.props.Serialize());

			string cfgFile = file + ".wz";
			if (!ulib::File::write(cfgFile, resourceProps.Serialize(), true))
			{
				WZ_THROW(ResourceFileAccessException, cfgFile);
			}
		}
		catch (const Exception& e)
		{
			WZ_CORE_ERROR("Error when saving config file:");
			WZ_CORE_ERROR(e.GetUnhandledMessage());
			WZ_CORE_WARN("Configuration was not saved");
		}

		try
		{
			if (!ulib::File::write(file, serialized, info.binary))
			{
				WZ_THROW(ResourceFileAccessException, file);
			}
		}
		catch (const Exception& e)
		{
			WZ_CORE_ERROR("Error when saving source file:");
			WZ_CORE_ERROR(e.GetUnhandledMessage());
			WZ_CORE_WARN("Source file was not saved");
		}

		DispatchEvent(ResourceSavedEvent(handle));
		WZ_CORE_INFO("Saved resource '{0}'", info.name());
	}

	void ResourceManagement::Unload(Resource::Handle handle)
	{
		__CHECK_HANDLE(handle, true);

		auto& info = s_resourceInfo[handle];

		WZ_CORE_TRACE("Unloading resource (path: {0}, handle: {1})", info.resPath, handle.id);

		delete s_resource[info.resourceIndex];
		s_resource[info.resourceIndex] = nullptr;

		DispatchEvent(ResourceUnloadedEvent(handle));
		WZ_CORE_INFO("Unloaded resource '{0}'", info.resPath);
	}

	void ResourceManagement::Reload(Resource::Handle handle, const PropertyTable& props)
	{
		__CHECK_HANDLE(handle, true);

		auto& resourceInfo = s_resourceInfo[handle];
		resourceInfo.props = props;

		Reload(handle);
	}

	void ResourceManagement::Reload(Resource::Handle handle)
	{
		__CHECK_HANDLE(handle, true);

		auto& info = s_resourceInfo[handle];

		ResData currentSource;
		if (!ulib::File::read(s_resourceDir + info.resPath, &currentSource, info.binary))
		{
			WZ_THROW(ResourceFileAccessException, info.resPath);
		}

		info.source = currentSource;

		string meta;
		if (!ulib::File::read(s_resourceDir + info.resPath + ".wz", &meta))
		{
			WZ_THROW(ResourceFileAccessException, info.resPath + ".w");
		}

		info.props.Clear();
		
		try
		{
			info.props.Deserialize(meta);
		}
		catch (const ParseErrorException& e)
		{
			WZ_CORE_ERROR("Reload(): Failed loading new import properties for {0}, using currently loaded properties instead", info.name());
		}

		Unload(handle);
		Load(handle);
	}

	void ResourceManagement::Delete(Resource::Handle handle)
	{
		auto& info = s_resourceInfo[handle];
		WZ_CORE_TRACE("Deleting resource (path: {0}, handle: {1})", info.resPath, handle.id);

		if (!IsValid(handle))
		{
			WZ_THROW(ResourceInvalidHandleException, handle); 
		}

		if (IsLoaded(handle))
		{
			Unload(handle);
		}

		DispatchEvent(ResourceDeletedEvent(handle));

		delete s_resource[info.resourceIndex];
		s_resource[info.resourceIndex] = nullptr;
		s_freeIndicesCount++;

		s_handles.erase(handle);
		string resPath = s_resourceInfo[handle].resPath;
		s_resourceInfo.erase(handle);

		WZ_CORE_INFO("Deleted resource '{0}'", resPath);
	}

	void ResourceManagement::SetResourceDir(const string& dir)
	{
		if (!ulib::Directory::exists(dir))
		{
			WZ_THROW(ResourceInvalidPathException, dir);
		}
		s_resourceDir = dir;
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

			if (IsLoaded(handle))
			{
				auto lastWriteTime = std::filesystem::last_write_time(s_resourceDir + info.resPath);
				
				if (lastWriteTime > info.lastWrite)
				{
					info.lastWrite = lastWriteTime;
					WZ_CORE_WARN("Changed detectedfor file {0}", info.resPath);
					DispatchEvent(ResourceFileChangedEvent(handle));
				}
			}
		}
	}

	const ResourceInfo& ResourceManagement::GetInfoFor(Resource::Handle handle)
	{
		if (IsValid(handle))
		{
			return s_resourceInfo[handle];
		}
		else
		{
			WZ_THROW(ResourceInvalidHandleException, handle);
		}
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
		Application::Get().OnEvent(e);
	}

	bool ResourceManagement::IsValid(Resource::Handle handle)
	{
		return s_handles.find(handle) != s_handles.end() && s_resourceInfo.find(handle) != s_resourceInfo.end();
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

	bool ResourceManagement::IsLoaded(Resource::Handle handle)
	{
		return IsValid(handle) && s_resource[s_resourceInfo[handle].resourceIndex] != nullptr;
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

	void ResourceManagement::ForEach(std::function<void(const Resource::Handle&)> fn)
	{
		for (const auto& hnd : s_handles)
		{
			fn(hnd);
		}
	}

}