#pragma once

namespace Wizzy {
    class IResource {
    public:

        IResource(const string& sourceFile)
            : m_sourceFile(sourceFile) {}
        virtual ~IResource() {}

        virtual void Load() = 0;
        virtual void Unload() = 0;
        virtual void Reload() = 0;
        virtual void Save() = 0;
        virtual bool IsGarbage() const = 0;

        inline const u32& GetResourceId() const { return m_id; }
        inline const string& GetSourceFile() const { return m_sourceFile; }

    protected:
        bool        m_isGarbage = true;

    private:
        string      m_sourceFile;
        u32         m_id = s_idCounter++;

    private:
        static u32  s_idCounter;
    };
}
