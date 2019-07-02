#include "wzpch.h"

#include "Wizzy/Resource/ResourceManagement.h"

namespace Wizzy {
    std::set<string> ResourceManagement::s_resourceAliases;
    std::unordered_map<string, IResource*> ResourceManagement::s_resources;
    std::set<ResourceHandle> ResourceManagement::s_resourceHandles;
    string ResourceManagement::s_resourcePath("");
    std::unordered_map<ResourceHandle, string> ResourceManagement::s_aliasesByHandle;
    ulib::Bitset ResourceManagement::s_emptyFlagSet;

    void ResourceManagement::Delete(const string& alias) {
        auto _target = Get<IResource>(alias);

		WZ_CORE_ASSERT(_target, "Tried deleting resource with alias '" + alias + "', but no such resource was found.");

        s_resourceAliases.erase(alias);
        s_resources.erase(alias);
        s_aliasesByHandle.erase(_target->GetResourceHandle());
        s_resourceHandles.erase(_target->GetResourceHandle());

        if (!_target->IsGarbage()) _target->Unload();

    }

    void ResourceManagement::Rename(const string& oldAlias,
                                    string newAlias) {
        WZ_CORE_TRACE("Renaming old alias '{0}' to new alias '{1}'",
                                        oldAlias,           newAlias);
        if (!Get<IResource>(oldAlias)) return;

        s_resourceAliases.erase(oldAlias);
        newAlias = FindNextAvailableAlias(newAlias);
        s_resourceAliases.emplace(newAlias);

        s_resources[newAlias] = s_resources[oldAlias];
        s_resources[oldAlias] = nullptr;
    }

    void ResourceManagement::Flush(const bool& save) {
		if (save) {
			for (const string& _alias : s_resourceAliases) {
				auto _resource = s_resources[_alias];
				if (!_resource->IsGarbage()) _resource->Save();
			}
		}
		UnloadAll();
		ClearGarbage();
    }

	void ResourceManagement::ClearGarbage() {
		for (const string& _alias : s_resourceAliases)
			if (s_resources[_alias]->IsGarbage()) Delete(_alias);
	}

    string ResourceManagement::FindNextAvailableAlias(const string& takenAlias) {
        if (!Get<IResource>(takenAlias)) return takenAlias;

        string _freeAlias = string(takenAlias);

        u32 _num = 0;
        while (Get<IResource>(_freeAlias + " (" + std::to_string(++_num) + ")"));

        return _freeAlias;
    }

	void ResourceManagement::UnloadAll() {
		for (const string& _alias : s_resourceAliases) {
			auto _resource = s_resources[_alias];
			if (!_resource->IsGarbage()) _resource->Unload();
		}
	}

}