#include "wzpch.h"

#include "Wizzy/Scripting/Script.h"
#include "Wizzy/Utils.h"

namespace Wizzy {

    Script::Script(const string& data, Flagset flags)
        : Resource(flags, "Script", WZ_EXTENSION_SCRIPT) {

        if (flags.GetBit(SCRIPT_LUA)) {
            m_scriptType = SCRIPT_LUA;
            m_scriptTypeStr = "LUA";
        } else if (flags.GetBit(SCRIPT_PYTHON)) {
            m_scriptType = SCRIPT_PYTHON;
            m_scriptTypeStr = "PY";
        } else {
            WZ_CORE_ERROR("Script initialization error: no script type was set in flags");
        }

        m_isValid = m_scriptType != SCRIPT_INVALID;

        if (m_isValid) {
            WZ_CORE_INFO("Initialized a script of type '{0}'", m_scriptTypeStr);
        }
    }
}
