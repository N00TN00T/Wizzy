
#include "Wizzy/Resources/ResourceManagement.h"

namespace Wizzy {
    std::set<string> ResourceManagement::s_resourceAliases;
    std::unordered_map<string, IResource*> ResourceManagement::s_resources;
    string ResourceManagement::s_resourcePath("");

    void ResourceManagement::Delete(const string& alias) {
        auto _target = Get<IResource>(alias);

        if (!_target) return;

        if (!_target->IsGarbage()) _target->Unload();
        delete _target;

        s_resourceAliases.erase(alias);
        s_resources[alias] = nullptr;
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
        for (const string& _alias : s_resourceAliases) {
            auto _resource = s_resources[_alias];
            if (save) _resource->Save();
            Delete(_alias);
        }
    }

    string ResourceManagement::FindNextAvailableAlias(const string& takenAlias) {
        if (!Get<IResource>(takenAlias)) return takenAlias;

        string _freeAlias = string(takenAlias);

        u32 _num = 0;
        while (Get<IResource>(_freeAlias + " (" + std::to_string(++_num) + ")"));

        return _freeAlias;
    }

}
