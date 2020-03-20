#pragma once

#include "Wizzy/WizzyExceptions.h"
#include "Wizzy/Resource/Resource.h"

/*

Intended way of use:

Instead of passing around pointers to resources they are associated with handles which
can be passed around safely without accidentally touching garbage memory. The handle is
associated with info about the resource, such as the path, name, props and resource index.
This means that you may have a valid handle that points to a unloaded Resource so it's
important to always make sure the resource is loaded before redeeming a handle to retrieve
the associated resource.

Handles are like pointers, if you lose them you have no way of accessing the memory
they are associated with.

To load a NEW resource from a file that is NOT in the resource directory
	1.	Call AddToResourceDir<T>(string file, string resPath, PropertyLibrary props)
			- 'file' is the file to be copied to the resource directory
			- 'resPath' is the relative path from the resource directory as root
			- 'props' is the import configuration
			- The return value is a Resource::Handle where T is the resource class type
	2.	Call Load(Resource::Handle handle)
		- 'handle' is the handle registered and returned from AddToResourceDir()

To load a resource that's already in the resource dir but not yet registered to a handle
	Call Load(string resPath, uId id, PropertyLibrary defaultProps)
		- 'resPath' is the relative resource directory path to a file that should exist
		- 'id' is the id to be assigned to the handle
		- 'defaultProps' is the props to be assigned if there aren't any serialized ones found
*/

namespace Wizzy
{
	typedef Resource* (*ResourceCreateFn)(const ResData&, const PropertyLibrary&); //td::function<Resource * (string, const PropertyLibrary&)> ResourceCreateFn;

	struct ResourceInfo
	{
		string				resPath;
		u32					resourceIndex;
		PropertyLibrary		props;
		ResourceCreateFn	createFn;
		ResData				source;
		string				type;
		bool				runtime = false;

		inline string name() const 
		{ 
			static string cache = "";
			static string lastPath = "";

			if (resPath != lastPath)
			{
				cache = ulib::File::name_of(resPath);
			}

			lastPath = resPath;
			return cache;
		}
	};

	class ResourceManagement
	{
	public:
		/*static void LoadResourceList(const string& listFile);
		static void WriteResourceList(const string& listFile);*/
		// Add file to resource directory and register it to a handle. Will NOT load the resource.
		template <typename T>
		static typename T::Handle  AddToResourceDir(const string& file, string resPath, const PropertyLibrary& props);

		template <typename T>
		static typename T::Handle AddRuntimeResource(T* resource, const PropertyLibrary& props = PropertyLibrary());

		// Add a resource already in memory and register to a handle
		template <typename T>
		static typename T::Handle AddResource(T* resource, const string& resPath, const PropertyLibrary& props);

		// Load a file already in the resource directory and register it to a handle
		template <typename T>
		static typename T::Handle Load(const string& resPath, uId id = 0, const PropertyLibrary& defaultProps = PropertyLibrary());

		// Load a file associated with given handle
		static void Load(Resource::Handle handle);

		// Overwrite file data with the serialized data of the resource in memory
		static void Save(Resource::Handle handle);

		static void Unload(Resource::Handle handle);

		static void Reload(Resource::Handle handle, const PropertyLibrary& props);
		static void Reload(Resource::Handle handle);

		static void Delete(Resource::Handle handle);

		template <typename T>
		static T& Get(typename T::Handle handle);
		template <typename T>
		static bool TryGet(typename T::Handle handle, T*& outResource);

		// Returns true if handle is registered correctly
		static bool IsValid(Resource::Handle handle);
		// Returns true if handle to file is registered correctly
		static bool IsValid(const string& resPath);
		// Returns true if handle is valid an associated resource is loaded
		static bool IsLoaded(Resource::Handle handle);

		// Returns the handle for either resPath or name
		static Resource::Handle HandleOf(const string& str);

		inline static const std::set<Resource::Handle>& GetHandles() { return s_handles; }

		template <typename T>
		static void ForEach(std::function<void(const Resource::Handle&)> fn);
		static void ForEach(std::function<void(const Resource::Handle&)> fn);

		static void SetResourceDir(const string& dir);
		inline static const string& GetResourceDir() { return s_resourceDir; }

		// Finds errors and throws exceptions for them
		static void Validate(bool checkSources = false);

		static const ResourceInfo& GetInfoFor(Resource::Handle handle);

	private:
		template <typename T>
		static typename T::Handle Register(string resPath, const PropertyLibrary& props);
		template <typename T>
		static typename T::Handle RegisterWithId(string resPath, uId id, const ResData& fileData, const PropertyLibrary& props, bool runtime = false);

		static string UniquenizePathName(const string& path);

	private:
		static std::vector<Resource*> s_resource;
		static Resource::HandleMap<ResourceInfo> s_resourceInfo;
		static std::set<Resource::Handle> s_handles;
		static string s_resourceDir;
		static u32 s_freeIndicesCount;
		static uId s_idCounter;
	};

	template<typename T>
	inline typename T::Handle ResourceManagement::AddToResourceDir(const string& file, string resPath, const PropertyLibrary& props)
	{
		WZ_CORE_TRACE("Adding resource to directory from '{0}' to respath '{1}'", file, resPath);
		resPath = UniquenizePathName(resPath);
		WZ_CORE_TRACE("Final respath: {0}", resPath);

		string fullPath = s_resourceDir + resPath;

		string errReason = "";
		if (!ulib::Directory::create_all(ulib::File::directory_of(fullPath), errReason))
		{
			WZ_THROW(Exception, "Failed creating directories in '" + fullPath + "' Reason: " + errReason);
		}
		if (!ulib::File::copy(file, fullPath))
		{
			WZ_THROW(Exception, "Could not copy file '" + file + "' to '" + fullPath + "'");
		}

		WZ_CORE_TRACE("Copied file to resource dir, now registering it...");
		
		return Register<T>(resPath, props);
	}

	template<typename T>
	inline typename T::Handle ResourceManagement::AddRuntimeResource(T* resource, const PropertyLibrary& props)
	{
		auto hnd = RegisterWithId<T>("runtime/", ++s_idCounter, ResData(), props, true);
		s_resource[GetInfoFor(hnd).resourceIndex] = resource;
		return hnd;
	}

	template<typename T>
	inline typename T::Handle ResourceManagement::AddResource(T* resource, const string& resPath, const PropertyLibrary& props)
	{
		WZ_CORE_ASSERT(resource != nullptr, "Cannot add a null resource");

		ResData serialized = resource->Serialize();

		string resFile = s_resourceDir + resPath;

		if (ulib::File::exists(resFile))
		{
			resFile = UniquenizePathName(resFile);
		}

		if (!ulib::File::write(resFile, serialized))
		{
			WZ_THROW(ResourceFileAccessException, resFile);
		}

		auto handle = Register<T>(resPath, props);

		auto& info = s_resourceInfo[handle];
		static std::mutex mutex;
		{
			std::lock_guard<std::mutex> lock(mutex);
			s_resource[info.resourceIndex] = resource;
		}
		
		info.source = serialized;

		return handle;
	}

	template<typename T>
	inline typename T::Handle ResourceManagement::Load(const string& resPath, uId id, const PropertyLibrary& props)
	{
		if (id == 0) id = ++s_idCounter;
		
		// Look for config file and if it exists, try to deserialize to props and send instead

		WZ_CORE_TRACE("Loading a resource already in reasource dir");
		PropertyLibrary cfgProps = props;

		string configFile = s_resourceDir + resPath + ".wz";
		if (ulib::File::exists(configFile))
		{
			WZ_CORE_TRACE("Config file found: {0}", configFile);
			string cfgData = "";
			if (!ulib::File::read(configFile, &cfgData, true))
			{
				WZ_THROW(ResourceFileAccessException, configFile);
			}
			
			try
			{
				cfgProps.Deserialize(cfgData);
				WZ_CORE_TRACE("Config file deserialized");
			}
			catch (const Exception& e)
			{
				WZ_CORE_ERROR("Failed to deseralize config file {0}", configFile);
				WZ_CORE_ERROR("    Reason: {0}", e.GetMessage());
			}
		}

		ResData fileData;
		if (!ulib::File::read(s_resourceDir + resPath, &fileData, true))
		{
			WZ_THROW(ResourceFileAccessException, resPath);
		}
		auto handle = RegisterWithId<T>(resPath, id, fileData, cfgProps);
		Load(handle);

		return handle;
	}

	template<typename T>
	inline T& ResourceManagement::Get(typename T::Handle handle)
	{
		WZ_CORE_ASSERT(IsValid(handle) && IsLoaded(handle), "Tried retrieving resource by invalid handle");

		return (T&)*s_resource.at(s_resourceInfo[handle].resourceIndex);
	}

	template<typename T>
	inline bool ResourceManagement::TryGet(typename T::Handle handle, T*& outResource)
	{
		if (IsValid(handle) && IsLoaded(handle))
		{
			outResource = (T*)s_resource[s_resourceInfo[handle].resourceIndex];
			return true;
		}
		outResource = NULL;
		return false;
	}

	template<typename T>
	inline void ResourceManagement::ForEach(std::function<void(const Resource::Handle&)> fn)
	{
		for (const auto& hnd : s_handles)
		{
			WZ_DEBUG("{0}, {1}", typestr(T), GetInfoFor(hnd).type);
			if (typestr(T) == GetInfoFor(hnd).type)
			{
				fn(hnd);
			}
		}
	}

	template<typename T>
	inline typename T::Handle ResourceManagement::Register(string resPath, const PropertyLibrary& props)
	{
		WZ_CORE_TRACE("Registering a resource of type '{0}' to path '{1}'", typestr(T), resPath);

		if (!ulib::File::exists(s_resourceDir + resPath))
		{
			WZ_THROW(ResourceInvalidPathException, resPath);
		}

		ResData fileData;
		if (!ulib::File::read(s_resourceDir + resPath, &fileData, true))
		{
			WZ_THROW(ResourceFileAccessException, resPath);
		}

		return RegisterWithId<T>(resPath, ++s_idCounter, fileData, props);
	}

	template<typename T>
	inline typename T::Handle ResourceManagement::RegisterWithId(string resPath, uId id, const ResData& fileData, const PropertyLibrary& props, bool runtime)
	{
		WZ_CORE_TRACE("Creating a handle with ID {0}", id);

		auto handle = T::Handle(id);

		ResourceInfo info =
		{
			resPath,
			s_resource.size(),
			props,
			T::Create,
			fileData,
			typestr(T),
			runtime
		};

		info.props.SetProperty("SourceFile", resPath);
		info.props.SetProperty("Type", typestr(T));

		if (s_freeIndicesCount > 0)
		{
			for (int32 i = 0; i < s_resource.size(); i++)
			{
				if (s_resource[i] == nullptr)
				{
					info.resourceIndex = i;
					s_freeIndicesCount--;
					break;
				}
			}
		}
		else
		{
			s_resource.push_back(nullptr);
		}
		WZ_CORE_ASSERT(s_handles.emplace(handle).second, "Handle already exists, make sure to load resource list before adding new resources");
		
		static std::mutex mutex;
		{
			std::lock_guard<std::mutex> lock(mutex);
			s_resourceInfo[handle] = info;
			s_idCounter = std::max(s_idCounter, id + 1);
		}

		if (info.runtime)
		{
			WZ_CORE_INFO("Registered resource (RUNTIME):");
		}
		else
		{
			WZ_CORE_INFO("Registered resource:");
		}
		Log::SetExtra(false);
		WZ_CORE_INFO("    Type:    {0}", typestr(T));
		WZ_CORE_INFO("    Respath: {0}", resPath);
		WZ_CORE_INFO("    Id:      {0}", handle.id);
		Log::SetExtra(true);

		return handle;
	}
	
}


