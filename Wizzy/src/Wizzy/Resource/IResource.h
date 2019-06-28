#pragma once

namespace Wizzy {

    #define WZ_NULL_RESOURCE_HANDLE -1
    #define WZ_IS_RESOURCE_HANDLE_NULL(h) (h == WZ_NULL_RESOURCE_HANDLE || h <= 0)

    typedef int32 ResourceHandle;

    class IResource {
    public:

        IResource(const string& sourceFile, const string& resourceType);
        virtual
        ~IResource() {}

        virtual
        void Load() = 0;
        virtual
        void Unload() = 0;
        virtual
        void Reload() = 0;
        virtual
        void Save() = 0;
        virtual
        bool IsGarbage() const = 0;

        inline
        ResourceHandle GetResourceHandle() const { return m_handle; }
        inline
        const string& GetSourceFile() const { return m_sourceFile; }
        inline
        const string& GetType() const { return m_resourceType; }

    protected:
        bool            m_isGarbage = true;

    private:
        string          m_sourceFile;
        string          m_resourceType;
        ResourceHandle  m_handle = ++s_handleCounter;

    private:
        static ResourceHandle s_handleCounter;
    };
}
