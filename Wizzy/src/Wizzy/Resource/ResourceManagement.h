 #pragma once

#include "Wizzy/Resource/Resource.h"
#include "Wizzy/Events/Event.h"

/*

Ownership of ALL resources imported with or created in ResourceManagement
belongs to ResourceManagement. The only correct way to unload/free
these resources is by calling Unload(string alias) or UnloadAll()

*/

namespace Wizzy {

    typedef std::set<string>::iterator FileIdx;
    typedef std::function<bool(Event&)> ResourceEventCallbackFn;

    class ResourceManagement {
    public:
        ResourceManagement() = delete;

        inline static
        void SetEventCallbackFn(ResourceEventCallbackFn fn) {
            s_callbackFn = fn;
        }

/*
        Create and add a resource of given type from given file and assign the
        handle that will be used to retrieve the resource. Ownership of
        allocated memory will belong to resource management. Also send in a
        bitset of flags allowing broader customization of the resource settings.
*/
        template <typename TResource>
        static
        void Load(string file, const ResourceHandle& handle,
                  Flagset flags = Flagset());

/*
        Add an already created resource and associate it, and given file, with
        given handle. Calling this will give ownership of the resource memory
        to resource management, meaning it will and should only be freed
        through resource management.
*/
        static
        bool Add(Resource *resource, const ResourceHandle& handle);

/*
        Unload the resource associated with given handle from memory and remove
        it.
*/
        static
        void Unload(const ResourceHandle& handle);

/*
        Unload and load the resource associated with given handle.
*/
        template <typename TResource>
        static
        void Reload(const ResourceHandle& handle);

/*
        Serialize the resource associated with given handle (by calling
        per-resource defined 'Serialize()') and write the data to the file
        associated with the given handle.
*/
        static
        void Save(const ResourceHandle& handle);

/*
        Unload and remove all resource and, optionally, save them beforehand.
*/
        static
        void Flush(bool save = false);

/*
        Retrieve and return the file associated with given handle.
*/
        static
        const string& FileOf(const ResourceHandle& handle);

/*
        Check whether or not given handle is valid. If no resource is associated
        with given handle, or it's a null handle, it's invalid.
*/
        static
        bool IsValid(const ResourceHandle& handle);

/*
        Check whether or not given handle is of given type. Returns false if
        given handle is invalid.
*/
        template <typename TResource>
        static
        bool Is(const ResourceHandle& handle);

/*
        Return the resource associated with given handle, if that resource is
        of given type.
*/
        template <typename TResource>
        static
        TResource* Get(const ResourceHandle& handle);

        static
        void Rename(const ResourceHandle& oldHandle, const ResourceHandle& newHandle);

        inline static
        void SetResourceDirectory(const string& dir) { s_resourceDir = dir; }

        inline static
        const std::set<ResourceHandle>& GetHandles() { return s_handles; }

    private:
        static
        std::set<ResourceHandle> s_handles;
        static
        std::set<string> s_files;
        static
        std::unordered_map<ResourceHandle, FileIdx> s_fileIndices;
        static
        std::unordered_map<ResourceHandle, Resource*> s_resources;
        static
        string s_resourceDir;
        static
        ResourceEventCallbackFn s_callbackFn;
    };

    template <typename TResource>
    inline
    void ResourceManagement::Load(string file, const ResourceHandle& handle, Flagset flags) {
        WZ_CORE_TRACE("Loading resource of type '{0}' from file '{1}' with handle '{2}' and passing a set of flags", typestr(TResource), file, handle);
        WZ_CORE_ASSERT(ulib::File::exists(file), "File '" + file + "' does not exist");

        string _data = "";
        bool _readSuccess = ulib::File::read(file, &_data);
        WZ_CORE_ASSERT(_readSuccess, "Failed reading file");

        TResource *_resource = TResource::Create(file, _data, flags);

        if (!ResourceManagement::Add(_resource, handle)) {
            WZ_CORE_ERROR("Failed adding loaded file, file unloaded and nothing changed");
            delete _resource;
        }
    }

    template <typename TResource>
    void ResourceManagement::Reload(const ResourceHandle& handle) {
        WZ_CORE_TRACE("Reloading resource associated with handle '{0}'...", handle);
        WZ_CORE_ASSERT(ResourceManagement::IsValid(handle), "Tried unloading invalid handle");

        string _file = ResourceManagement::FileOf(handle);
        Flagset flags = s_resources[handle]->GetFlags();

        Unload(handle);
        Load<TResource>(_file, handle, flags);
    }

    template <typename TResource>
    inline
    bool ResourceManagement::Is(const ResourceHandle& handle) {
        return ResourceManagement::IsValid(handle) &&
               dynamic_cast<TResource*>(s_resources[handle]) != nullptr;
    }

    template <typename TResource>
    inline
    TResource* ResourceManagement::Get(const ResourceHandle& handle) {
        WZ_CORE_ASSERT(ResourceManagement::Is<TResource>(handle), "Tried getting resource by invalid handle or wrong type (handle: '" + handle + "', type: '" + string(typestr(TResource)) + "')");
        return dynamic_cast<TResource*>(s_resources[handle]);
    }
}
