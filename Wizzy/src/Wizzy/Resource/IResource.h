#pragma once

namespace Wizzy {
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

        const u32& GetResourceId() const { return m_id; } 
        inline
        const string& GetSourceFile() const { return m_sourceFile; }
        inline
        const string& GetType() const { return m_resourceType; }

    protected:
        bool        m_isGarbage = true;

    private:
        string      m_sourceFile;
        string      m_resourceType;
        u32         m_id = s_idCounter++;

    private:
        static u32  s_idCounter;
    };
}
