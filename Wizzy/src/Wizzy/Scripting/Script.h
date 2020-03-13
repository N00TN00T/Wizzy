#pragma once

#include "Wizzy/Resource/Resource.h"

namespace Wizzy {

	class ScriptContext;

	inline string ScriptEnumToString(ScriptEnum v) {
		switch (v) {
		case SCRIPT_TYPE_CONFIG: return "CONFIG";
		case SCRIPT_TYPE_SYSTEM: return "SYSTEM";
		case SCRIPT_TYPE_COMPONENT: return "COMPONENT";
		default: break;
		}

		WZ_CORE_ASSERT(false, "Invalid or unhandeled script enum");
		return "";
	}

	inline ScriptEnum StringToScriptEnum(const string& s) {

		if (s == "CONFIG") return SCRIPT_TYPE_CONFIG;
		else if (s == "SYSTEM") return SCRIPT_TYPE_SYSTEM;
		else if (s == "COMPONENT") return SCRIPT_TYPE_COMPONENT;

		WZ_CORE_ASSERT(false, "Invalid or unhandeled ScriptEnum String '{0}'", s);

		return SCRIPT_TYPE_NONE;
	}

    class Script
        : public Resource {
    public:

        Script(const ResData& data, const PropertyLibrary& props);

        inline virtual ResData Serialize() const override {
			return ResData((const byte*)m_sourceCode.data(), (const byte*)m_sourceCode.data() + m_sourceCode.size());
        }

		static const PropertyLibrary& GetTemplateProps();

		bool Do();

		luabridge::LuaRef Get(const string& name);

        inline const string&		GetScriptCode() const { return m_sourceCode; }
        inline const ScriptEnum&    GetScriptType() const { return m_scriptType; }

        inline static Resource* Create(const ResData& data, const PropertyLibrary& props) {
            return new Script(data, props);
        }

    private:
        string                  m_sourceCode = "";
		ScriptEnum				m_scriptType;
	private:
		static PropertyLibrary* s_templateProps;
	};
}
