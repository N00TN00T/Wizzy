#pragma once

#include "Wizzy/Exceptions/WizzyExceptions.h"
#include "Wizzy/Resource/Resource.h"
#include "Wizzy/Events/ResourceEvent.h"

namespace Wizzy
{
	typedef Resource* (*ResourceCreateFn)(const ResData&, const PropertyTable&); //td::function<Resource * (string, const PropertyTable&)> ResourceCreateFn;

	struct ResourceInfo
	{
		string				resPath;
		u32					resourceIndex;
		PropertyTable		metaData;
		ResourceCreateFn	createFn;
		ResData				source;
		string				type;
		bool				runtime = false;
		bool				binary;
		bool				isUnloading = false;
		bool				isDeleting = false;
		std::filesystem::file_time_type lastWrite;
		uId id;

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

		template <typename T, typename ...Targ>
		static typename T::Handle Create(const string& resPath, const Targ&... arg);

		template <typename T, typename ...Targ>
		static typename T::Handle CreateRuntime(const Targ&... arg);

		// Add file to resource directory and register it to a handle. Will NOT load the resource.
		template <typename T>
		static typename T::Handle  AddToResourceDir(const string& file, string resPath, const PropertyTable& props);

		template <typename T>
		static typename T::Handle AddRuntimeResource(T* resource, const PropertyTable& props = PropertyTable());

		// Add a resource already in memory and register to a handle
		template <typename T>
		static typename T::Handle AddResource(T* resource, const string& resPath, const PropertyTable& props);

		// Load a file already in the resource directory and register it to a handle
		template <typename T>
		static typename T::Handle Load(const string& resPath, const PropertyTable& defaultProps = PropertyTable());

		// Load a file associated with given handle
		static void Load(const Resource::Handle& handle);

		// Overwrite file data with the serialized data of the resource in memory
		static void Save(const Resource::Handle& handle);

		static void Unload(const Resource::Handle& handle);

		static void Reload(const Resource::Handle& handle, const PropertyTable& props);
		static void Reload(const Resource::Handle& handle);

		static void Delete(const Resource::Handle& handle);

		template <typename T>
		static T& Get(const typename T::Handle& handle);
		template <typename T>
		static bool TryGet(const typename T::Handle& handle, T*& outResource);

		// Returns true if handle is registered correctly
		static bool IsValid(const Resource::Handle& handle);
		// Returns true if handle to file is registered correctly
		static bool IsValid(const string& resPath);
		// Returns true if handle is valid an associated resource is loaded
		static bool IsLoaded(const Resource::Handle& handle);

		static bool IsFileRegistered(const string& resFile);

		template <typename T>
		inline static bool Is(const Resource::Handle& hnd);

		// Returns the handle for either resPath or name
		static Resource::Handle HandleOf(const string& str);
		
		static string NameOf(const Resource::Handle& hnd);

		inline static std::set<Resource::Handle> GetHandles() { return s_handles; }

		template <typename T>
		static void ForEach(std::function<bool(const typename T::Handle&)> fn);
		static void ForEach(std::function<bool(const Resource::Handle&)> fn);

		static void SetResourceDir(const string& dir);
		inline static const string& GetResourceDir() { return s_resourceDir; }

		// Finds errors and throws exceptions for them
		static void Validate(bool checkSources = false);

		static const ResourceInfo& GetInfoFor(const Resource::Handle& handle);
		static PropertyTable& GetMetaData(const Resource::Handle& handle);

		static Resource::Handle IdToHandle(uId id);


		static void _Update();

	private:
		template <typename T>
		static typename T::Handle Register(string resPath, const PropertyTable& props, bool runtime = false);

		static string UniquenizePathName(const string& path);

		static void DispatchEvent(ResourceEvent& e);

		static void WriteMeta(const Resource::Handle& hnd);
		static PropertyTable ReadMeta(const Resource::Handle& hnd);
		static PropertyTable ReadMeta(const string& resPath);
		static bool HasMeta(const Resource::Handle& hnd);
		static bool HasMeta(const string& resPath);

		typedef std::function<void()> ResourceAction;
		static void DeferAction(ResourceAction action);

	private:
		static std::vector<Resource*> s_resource;
		static Resource::HandleMap<ResourceInfo> s_resourceInfo;
		static std::set<Resource::Handle> s_handles;
		static string s_resourceDir;
		static u32 s_freeIndicesCount;
		static uId s_idCounter;
		static string s_idFile;
		
		static std::queue<ResourceAction> s_deferredActions;
	};

	template <typename T, typename ...Targ>
	inline typename T::Handle ResourceManagement::Create(const string& resPath, const Targ&... arg)
	{
		T* res = (T*)T::Create(arg...);
		return AddResource<T>(res, resPath);
	}

	template <typename T, typename ...Targ>
	inline typename T::Handle ResourceManagement::CreateRuntime(const Targ&... arg)
	{
		T* res = (T*)T::Create(arg...);
		return AddRuntimeResource<T>(res);
	}

	template<typename T>
	inline typename T::Handle ResourceManagement::AddToResourceDir(const string& file, string resPath, const PropertyTable& props)
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
	inline typename T::Handle ResourceManagement::AddRuntimeResource(T* resource, const PropertyTable& props)
	{
		auto hnd = Register<T>("runtime/", props, true);
		s_resource[GetInfoFor(hnd).resourceIndex] = resource;
		DispatchEvent(ResourceLoadedEvent(hnd));
		return hnd;
	}

	template<typename T>
	inline typename T::Handle ResourceManagement::AddResource(T* resource, const string& resPath, const PropertyTable& props)
	{
		WZ_CORE_ASSERT(resource != nullptr, "Cannot add a null resource");

		ResData serialized = resource->Serialize();

		string resFile = s_resourceDir + resPath;

		if (ulib::File::exists(resFile))
		{
			resFile = UniquenizePathName(resFile);
		}

		if (!ulib::File::write(resFile, serialized, T::IsFileBinary()))
		{
			WZ_THROW(ResourceFileAccessException, resFile);
		}

		auto handle = Register<T>(resPath, props);

		auto& info = s_resourceInfo[handle];
		s_resource[info.resourceIndex] = resource;
		
		info.source = serialized;
		DispatchEvent(ResourceLoadedEvent(handle));
		return handle;
	}

	template<typename T>
	inline typename T::Handle ResourceManagement::Load(const string& resPath, const PropertyTable& props)
	{
		WZ_CORE_TRACE("Loading a resource already in reasource dir");

		auto handle = Register<T>(resPath, props);
		WZ_CORE_ASSERT(dynamic_cast<T::Handle*>(&handle), "Handle is incorrect type");
		Load(handle);

		return handle;
	}

	template<typename T>
	inline T& ResourceManagement::Get(const typename T::Handle& handle)
	{
		WZ_CORE_ASSERT(IsLoaded(handle), "Handle unloaded");
		WZ_CORE_ASSERT(Is<T>(handle), "Handle type mistmatch");

		return (T&)*s_resource.at(s_resourceInfo[handle].resourceIndex);
	}

	template<typename T>
	inline bool ResourceManagement::TryGet(const typename T::Handle& handle, T*& outResource)
	{
		if (IsValid(handle) && IsLoaded(handle))
		{
			outResource = dynamic_cast<T*>(s_resource[s_resourceInfo[handle].resourceIndex]);
			return outResource != NULL;
		}
		outResource = NULL;
		return false;
	}

	template<typename T>
	inline void ResourceManagement::ForEach(std::function<bool(const typename T::Handle&)> fn)
	{
		for (const auto& hnd : s_handles)
		{
			if (std::is_same<T, Resource>() || typestr(T) == GetInfoFor(hnd).type)
			{
				if (!fn(hnd)) break;
			}
		}
	}

	template<typename T>
	inline typename T::Handle ResourceManagement::Register(string resPath, const PropertyTable& props, bool runtime)
	{
		WZ_CORE_TRACE("Registering a resource of type '{0}' to path '{1}'", typestr(T), resPath);

		if (!ulib::File::exists(s_resourceDir + resPath) && !runtime)
		{
			WZ_THROW(ResourceInvalidPathException, resPath);
		}

		WZ_CORE_TRACE("Trying to read resource file if not runtime");
		ResData fileData;
		if (!ulib::File::read(s_resourceDir + resPath, &fileData, T::IsFileBinary()) && !runtime)
		{
			WZ_THROW(ResourceFileAccessException, resPath);
		}

		uId id = 0;

		WZ_CORE_TRACE("Looking for meta to determine ID");
		if (!runtime && HasMeta(resPath))
		{
			auto metaData = ReadMeta(resPath);

			if (metaData.Is<int32>("id"))
			{
				id = metaData.Get<int32>("id");
			}
		}

		if (id == 0)
		{
			id = ++s_idCounter;
		}

		WZ_CORE_TRACE("Creating a handle with ID {0}", id);

		auto handle = T::Handle(id);
		WZ_CORE_ASSERT(handle.Is<T>(), "Handle init error");
		ResourceInfo info =
		{
			resPath,
			s_resource.size(),
			props,
			T::Create,
			fileData,
			typestr(T),
			runtime,
			T::IsFileBinary(),
			false,
			false,
			std::filesystem::file_time_type::clock::now(),
			id
		};

		if (info.metaData.GetCount() == 0) info.metaData = T::GetTemplateProps();

		info.metaData.Set("resPath", resPath);
		info.metaData.Set("type", typestr(T));
		info.metaData.Set<int32>("id", id);

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

		

		bool alreadyExists = !s_handles.emplace(handle).second;

		WZ_CORE_ASSERT(!alreadyExists, 
			"Handle with id " + std::to_string(handle.id) + " already exists");

		s_resourceInfo[handle] = info;
		s_idCounter = std::max(s_idCounter, id + 1);

		if (!runtime)
		{
			WriteMeta(handle);
		}

		if (info.runtime)
		{
			WZ_CORE_TRACE("Registered resource (runtime):\n    Type:    {0}\n    ResPath: {1}\n    Id:      {2}",
				typestr(T), resPath, handle.id);
		}
		else
		{
			WZ_CORE_INFO("Registered resource:\n    Type:    {0}\n    ResPath: {1}\n    Id:      {2}",
				typestr(T), resPath, handle.id);
		}

		DispatchEvent(ResourceRegisteredEvent(handle));

#ifdef WZ_CONFIG_DEBUG
		while (!ulib::File::write(s_idFile, std::to_string(s_idCounter))) {
			WZ_CORE_ERROR("Id file '{}' is locked, issues may occur...", s_idFile);
			s_idFile += "_";
		}
#else
		bool readResult = ulib::File::write(s_idFile, std::to_string(s_idCounter));
		WZ_CORE_ASSERT(readResult, "Failed reading ID file");
#endif

		return handle;
	}

	template <typename T>
	inline static bool ResourceManagement::Is(const Resource::Handle& hnd)
	{
		return IsValid(hnd) && hnd.Is<T>();
	}
	
}


