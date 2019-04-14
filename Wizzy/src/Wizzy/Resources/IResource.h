#pragma once

namespace Wizzy {
    class IResource {
    public:
        inline const u32& GetResourceId() const { return m_id; }
    protected:

    private:
        u32         m_id = s_idCounter++;

    private:
        static u32  s_idCounter;
    };
}
