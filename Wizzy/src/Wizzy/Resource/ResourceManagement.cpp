#include "wzpch.h"

#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Events/ResourceEvent.h"

namespace Wizzy {

    std::set<ResourceHandle> ResourceManagement::s_handles;
    std::set<string> ResourceManagement::s_files;
    std::unordered_map<ResourceHandle, FileIdx> ResourceManagement::s_fileIndices;
    std::unordered_map<ResourceHandle, Resource*> ResourceManagement::s_resources;
    string ResourceManagement::s_resourceDir = BASE_DIR + string("ProjectResources/");
    ResourceEventCallbackFn ResourceManagement::s_callbackFn = [](Event& e) {
        WZ_CORE_WARN("Event callback fn has not been set in resource management");
        return false;
    };

    bool ResourceManagement::Add(Resource *resource, const ResourceHandle& handle) {
        WZ_CORE_ASSERT(resource != nullptr, "Cannot add null resource");
        WZ_CORE_TRACE("Adding a resource with handle '{0}' to file...", handle);
        string _fileName = handle + "." + resource->GetExtension();
        string _file = ulib::File::to_absolute(s_resourceDir + _fileName);
        WZ_CORE_TRACE("...{0}", _file);
        bool _fileExists = ulib::File::exists(_file);

        WZ_CORE_TRACE("Adding handle & file");
        auto _handleResult = s_handles.emplace(handle);
        auto _fileResult = s_files.emplace(_file);

        bool _anyError = !_handleResult.second || !_fileResult.second ||
                         _fileExists || !resource->IsValid();
        if (_anyError) {

            WZ_CORE_ERROR("Could not add file...");
            if (!_handleResult.second) {
                WZ_CORE_ERROR("...handle '{0}' is already taken", handle);
            }
            if (!_fileResult.second) {
                WZ_CORE_ERROR("...file '{0}' is already used by another resource", _file);
            } if (_fileExists) {
                WZ_CORE_ERROR("...file '{0}' already exists (but isn't used by another resource!)", _file);
            }
            if (!resource->IsValid()) {
                WZ_CORE_ERROR("resource was invalid after initialization");
            }
            return false;
        }
        WZ_CORE_TRACE("Binding resource to handle & file");
        s_fileIndices[handle] = _fileResult.first;
        s_resources[handle] = resource;

        string _data = resource->Serialize();
        bool _writeSuccess = ulib::File::write(_file, _data);
        WZ_CORE_ASSERT(_writeSuccess, "Failed writing file");

        auto _addEvent = ResourceAddEvent(handle, _file);
        s_callbackFn(_addEvent);

        WZ_CORE_INFO("Associated a resource with file '{0}' and handle '{1}'", _file, handle);
        return true;
    }

    void ResourceManagement::Unload(const ResourceHandle& handle) {
        WZ_CORE_TRACE("Unloading resource associated with handle '{0}'...", handle);
        WZ_CORE_ASSERT(ResourceManagement::IsValid(handle), "Tried unloading invalid handle");

        auto _deleteEvent = ResourceDeleteEvent(handle, FileOf(handle));
        s_callbackFn(_deleteEvent);

        delete s_resources[handle];

        WZ_CORE_TRACE("Removing all association with handle '{0}'...", handle);
        s_handles.erase(handle);
        s_files.erase(FileOf(handle));
        s_fileIndices.erase(handle);
        s_resources.erase(handle);
    }

    void ResourceManagement::Save(const ResourceHandle& handle) {
        WZ_CORE_TRACE("Saving resource associated with handle '{0}'...", handle);
        WZ_CORE_ASSERT(ResourceManagement::IsValid(handle), "Tried unloading invalid handle");

        string _data = s_resources[handle]->Serialize();

        if (!ulib::File::exists(FileOf(handle))) {
            WZ_CORE_WARN("File associated with handle '{0}' () no longer exists, writing anyways...", handle);
        }

        bool _writeSuccess = ulib::File::write(FileOf(handle), _data);
        WZ_CORE_ASSERT(_writeSuccess, "Failed writing resource data to file");

        auto _saveEvent = ResourceSaveEvent(handle, FileOf(handle));
        s_callbackFn(_saveEvent);

        WZ_CORE_INFO("Successfully saved resource with handle '{0}'", handle);
    }

    void ResourceManagement::Flush(bool save) {
        WZ_CORE_TRACE("Flushing resources...");

        auto _it = s_handles.begin();
        for (int32 i = s_handles.size() - 1; i >= 0; i--) {
            const ResourceHandle& _handle = *_it;
            if (save) {
                Save(_handle);
            }
            Unload(_handle);
            _it++;
        }

        WZ_CORE_INFO("Finished flushing resources");
    }

    const string& ResourceManagement::FileOf(const ResourceHandle& handle) {
        WZ_CORE_ASSERT(IsValid(handle), "Cannot retrieve file of invalid handle");
        return *s_fileIndices[handle];
    }

    bool ResourceManagement::IsValid(const ResourceHandle& handle) {
        return s_resources[handle] != nullptr &&
               handle != WZ_NULL_RESOURCE_HANDLE;
    }

    void ResourceManagement::Rename(const ResourceHandle& oldHandle, const ResourceHandle& newHandle) {
        WZ_CORE_TRACE("Renaming handle '{0}' to '{1}'", oldHandle, newHandle);
        if (!s_handles.emplace(newHandle).second) {
            WZ_CORE_ERROR("Could not rename '{0}' to '{1}' because '{1}' was already taken", oldHandle, newHandle);
            return;
        }

        s_fileIndices[newHandle] = s_fileIndices[oldHandle];
        s_resources[newHandle] = s_resources[oldHandle];

        s_handles.erase(oldHandle);
        s_fileIndices.erase(oldHandle);
        s_resources.erase(oldHandle);

        auto _renameEvent = ResourceRenameEvent(oldHandle, newHandle, FileOf(newHandle));
        s_callbackFn(_renameEvent);

        WZ_CORE_INFO("Renamed handle '{0}' to '{1}'", oldHandle, newHandle);
    }

}
