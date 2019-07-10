#pragma once

#include "Wizzy/Resource/Resource.h"

namespace Wizzy {

    enum {
        SCRIPT_LUA = 1, SCRIPT_PYTHON = 2, SCRIPT_INVALID = 0
    };

    class Script
        : public Resource {
    public:

        Script(const string& data, Flagset flags);

        inline virtual
        string Serialize() const override {
            return m_sourceCode;
        }

        inline const string&    GetScriptCode() const { return m_sourceCode; }
        inline const u32&       GetScriptType() const { return m_scriptType; }

        inline static Script* Create(const string& sourceFile,
                                     const string& data,
                                     const Flagset& flags) {
            return new Script(data, flags);
        }

    private:
        string                  m_sourceCode = "";
        u32                     m_scriptType = SCRIPT_INVALID;
        string                  m_scriptTypeStr = "";
    };
}
