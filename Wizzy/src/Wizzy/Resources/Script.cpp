#include "wzpch.h"

#include "Wizzy/Resources/Script.h"
#include "Wizzy/Utils.h"

namespace Wizzy {
    
    void Script::Load() {
        WZ_CORE_TRACE("Loading script from '{0}'...", GetSourceFile());
        WZ_CORE_ASSERT(File::read(this->GetSourceFile(), &m_sourceCode), "Failed reading script file");
        
        string _extension = File::extension_of(this->GetSourceFile());
        
        string _typeStr = "";
        
        if (_extension == ".lua") { m_scriptType = SCRIPT_LUA; _typeStr = "Lua"; }
        else if (_extension == ".py") { m_scriptType = SCRIPT_PYTHON; _typeStr = "Python"; }
        else m_scriptType = SCRIPT_INVALID;
        
        WZ_CORE_ASSERT(m_scriptType != SCRIPT_INVALID, "Script type is unknown or not a supported scripting language for Wizzy");
        
        WZ_CORE_INFO("Successfully loaded script, type is '{0}'", _typeStr);
    }
    void Script::Unload() {
        WZ_CORE_TRACE("Unloading script...");
        WZ_CORE_ASSERT(!this->IsGarbage(), "Tried unloading garbage-flagged Script");
        m_scriptType = SCRIPT_INVALID;
        m_sourceCode = "";
    }
    void Script::Reload() {
        WZ_CORE_TRACE("Reloading Script...");
        Unload();
        Reload();
    }
    void Script::Save() {
        WZ_CORE_TRACE("Saving script to file '{0}'", this->GetSourceFile());
        WZ_CORE_ASSERT(File::write(this->GetSourceFile(), m_sourceCode), "Failed writing to script file...");
    }
}