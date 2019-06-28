 #pragma once

#include "Wizzy/Resource/IResource.h"

/* Ownership of ALL resources imported with ResourceManagement
    belongs to ResourceManagement. The only correct way to unload
    these resources is by calling Unload(string alias) or UnloadAll() */

namespace Wizzy {
    class ResourceManagement {
    public:
        ResourceManagement() = delete;

    public:
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
        const std::set<string>& GetAliases() { return s_resourceAliases; }

    private:
        static
        string FindNextAvailableAlias(const string& takenAlias);
		static
        void UnloadAll();

    private:
        static
        std::set<string> s_resourceAliases;
        static
        std::unordered_map<string, IResource*> s_resources;
        static
        string s_resourcePath;
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
            s_resources[alias] = _resource;
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
            s_resources[alias] = _resource;
        } else {
            WZ_CORE_ERROR("Failed importing '{0}', alias '{1}' is already taken",
                                        typestr(TResource), alias);
        }

        return static_cast<TResource*>(s_resources[alias]);
    }

    template <typename TResource>
    inline TResource* ResourceManagement::Get(const string& alias) {
        if (auto _resource = dynamic_cast<TResource*>(s_resources[alias]))
            return _resource;
        else
            return nullptr;
    }
}
