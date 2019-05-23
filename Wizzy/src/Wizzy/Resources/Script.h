#pragma once

#include "Wizzy/Resources/IResource.h"

namespace Wizzy {
    
    enum {
        SCRIPT_LUA = 1, SCRIPT_PYTHON = 2, SCRIPT_INVALID = 0
    };
    
    class Script
        : public IResource {
    public:
    
        Script(const string& file)
            : IResource::IResource(file) {}
    
        virtual void            Load() override;
        virtual void            Unload() override;
        virtual void            Reload() override;
        virtual void            Save() override;
        
        inline virtual bool     IsGarbage() const override {
            return m_scriptType == SCRIPT_INVALID;
        }
        
        inline const string&    GetScriptCode() const { return m_sourceCode; }
        inline const u32&       GetScriptType() const { return m_scriptType; }
        
    private:
        string                  m_sourceCode = "";
        u32                     m_scriptType = SCRIPT_INVALID;
    };
}