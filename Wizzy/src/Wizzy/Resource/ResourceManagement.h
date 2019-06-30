 #pragma once

#include "Wizzy/Resource/IResource.h"

/* Ownership of ALL resources imported with or created in ResourceManagement
    belongs to ResourceManagement. The only correct way to unload/free
    these resources is by calling Unload(string alias) or UnloadAll() */

namespace Wizzy {
    class ResourceManagement {
    public:
        ResourceManagement() = delete;

    public:

        template <typename TResource>
        static
        void New(const string& newFile, const string& alias);
        template <typename TResource>
        static
        void New(const string& newFile,
                 const string& alias,
                 const ulib::Bitset& flags);

        template <typename TResource>
        static
        TResource* Import(const string& file,
                          const string& alias);
        template <typename TResource>
        static
        TResource* Import(const string& file,
                          const string& alias,
                          const ulib::Bitset& flags);

        inline static
        size_t ResourceCount() {
            return s_resourceAliases.size();
        }

        static
        void Delete(const string& alias);

        static
        void Rename(const string& oldAlias, string newAlias);

        static
        void Flush(const bool& save = false);

		static
        void ClearGarbage();

        template <typename TResource>
        static
        TResource* Get(const string& alias);
        template <typename TResource>
        static
        TResource* Get(ResourceHandle handle);

        template <typename TResource>
        inline static
        bool Is(const string& alias) {
            return dynamic_cast<TResource*>(s_resources[alias])
                   && !s_resources[alias]->IsGarbage();
        }
        template <typename TResource>
        inline static
        bool Is(ResourceHandle handle) {
            return dynamic_cast<TResource*>(s_resources[s_aliasesByHandle[handle]])
                   && !WZ_IS_RESOURCE_HANDLE_NULL(handle)
                   && s_resources[s_aliasesByHandle[handle]] != nullptr
                   && !s_resources[s_aliasesByHandle[handle]]->IsGarbage();
        }

        inline static
        void SetResourcePath(const string& path) {
            s_resourcePath = path;
        }

        template <typename TResource>
        inline static
        string AliasOf(TResource* resource) {
            for (const auto& _alias : s_resourceAliases) {
                if (s_resources[_alias] == resource) return _alias;
            }
            return "";
        }

        inline static
        string AliasOf(ResourceHandle handle) {
            return s_aliasesByHandle[handle];
        }

        inline static
        const std::set<string>& GetAliases() { return s_resourceAliases; }
        inline static
        const std::set<ResourceHandle> GetHandles() { return s_resourceHandles; }

    private:
        static
        string FindNextAvailableAlias(const string& takenAlias);
		static
        void UnloadAll();
        template <typename TResource>
        static
        void ImportInternal(const string& alias, TResource* resource);
        template <typename TResource>
        static
        void ProcessNewResource(const string& alias, TResource *resource);

    private:
        static
        std::set<string> s_resourceAliases;
        static
        std::set<ResourceHandle> s_resourceHandles;
        static
        std::unordered_map<ResourceHandle, string> s_aliasesByHandle;
        static
        std::unordered_map<string, IResource*> s_resources;
        static
        string s_resourcePath;
        static
        ulib::Bitset s_emptyFlagSet;
    };

    template <typename TResource>
    inline TResource* ResourceManagement::Import(const string& file,
                                                 const string& alias) {
        WZ_CORE_TRACE("Importing resource from file '{0}' as '{1}'",
                                                file, typestr(TResource));

        string _fullPath = s_resourcePath + file;

        bool _aliasFree = s_resourceAliases.emplace(alias).second;

        if (_aliasFree) {
            auto _resource = TResource::Create(_fullPath);
            _resource->Load();
            ResourceManagement::ImportInternal(alias, _resource);
        } else {
            WZ_CORE_ERROR("Failed importing '{0}', alias '{1}' is already taken",
                                        typestr(TResource), alias);
        }

        return static_cast<TResource*>(s_resources[alias]);
    }

    template <typename TResource>
    inline TResource* ResourceManagement::Import(const string& file,
                                                 const string& alias,
                                                 const ulib::Bitset& flags) {
        WZ_CORE_TRACE("Importing resource from file '{0}' as '{1}'",
                                                    file, typestr(TResource));

        string _fullPath = s_resourcePath + file;

        bool _aliasFree = s_resourceAliases.emplace(alias).second;

        if (_aliasFree) {
            auto _resource = TResource::Create(_fullPath, flags);
            _resource->Load();
            ResourceManagement::ImportInternal(alias, _resource);
        } else {
            WZ_CORE_ERROR("Failed importing '{0}', alias '{1}' is already taken",
                                        typestr(TResource), alias);
        }

        return static_cast<TResource*>(s_resources[alias]);
    }

    template <typename TResource>
    inline
    void ResourceManagement::ImportInternal(const string& alias, TResource* resource) {
        s_resources[alias] = resource;
        s_resourceHandles.emplace(resource->GetResourceHandle());
        s_aliasesByHandle[resource->GetResourceHandle()] = alias;
    }

    template <typename TResource>
    inline TResource* ResourceManagement::Get(const string& alias) {
        if (auto _resource = dynamic_cast<TResource*>(s_resources[alias]))
            return _resource;
        else
            return nullptr;
    }

    template <typename TResource>
    inline
    TResource* ResourceManagement::Get(ResourceHandle handle) {
        if (auto _resource = dynamic_cast<TResource*>(s_resources[s_aliasesByHandle[handle]]))
            return _resource;
        else
            return nullptr;
    }

    template <typename TResource>
    inline
    void ResourceManagement::New(const string& newFile, const string& alias) {
        if (ulib::File::exists(newFile)) {
            WZ_CORE_WARN("Could not create new resource of type '{0}', file '{1}' already exists",
                            typestr(TResource), newFile);
            return;
        }

        ProcessNewResource(TResource::New(newFile));
    }

    template <typename TResource>
    inline
    void ResourceManagement::New(const string& newFile,
                                 const string& alias,
                                 const ulib::Bitset& flags) {
        if (ulib::File::exists(newFile)) {
            WZ_CORE_WARN("Could not create new resource of type '{0}', file '{1}' already exists",
                            typestr(TResource), newFile);
            return;
        }

        ProcessNewResource(TResource::New(newFile, flags));
    }

    template <typename TResource>
    inline
    void ResourceManagement::ProcessNewResource(const string& alias,
                                                TResource *resource) {
        ResourceManagement::ImportInternal(alias, resource);
    }
}
