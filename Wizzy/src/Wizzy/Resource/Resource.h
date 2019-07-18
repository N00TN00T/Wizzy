#pragma once

namespace Wizzy {

    typedef string ResourceHandle;
    typedef ulib::Bitset Flagset;

    typedef ResourceHandle ShaderHandle;
    typedef ResourceHandle TextureHandle;
    typedef ResourceHandle ModelHandle;
    typedef ResourceHandle MaterialHandle;
    typedef ResourceHandle ScriptHandle;

    class Resource {
    public:

        Resource(const Flagset& flags,
                 const string& resourceType,
                 const string& extension);
        virtual
        ~Resource() {}

        virtual
        string Serialize() const = 0;
        inline
        const string& GetType() const { return m_resourceType; }
        inline
        const string& GetExtension() const { return m_extension; }
        inline
        Flagset& GetFlags() { return m_flags; }
        inline
        bool IsValid() const { return m_isValid; }

    protected:
        Flagset m_flags;
        bool m_isValid = false;

    private:
        string          m_resourceType;
        string          m_extension;
    };
}
