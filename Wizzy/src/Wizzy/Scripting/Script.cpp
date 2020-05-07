#include "wzpch.h"

#include "Wizzy/Scripting/Script.h"
#include "Wizzy/Utils.h"

namespace Wizzy {

	PropertyTable* Script::s_templateProps;

    Script::Script(const ResData& data, const PropertyTable& props) : Resource(props) 
	{
		m_resData = data;
    }
	Script::Script(const string& codeStr, const PropertyTable& props) : Resource(props)
	{
		m_scriptStr = codeStr;
	}

	const PropertyTable& Script::GetTemplateProps()
	{
		if (!s_templateProps)
		{
			s_templateProps = new PropertyTable;
			s_templateProps->Set("scriptLang", ScriptLangToString(script_lang_lua));
			
		}
		return *s_templateProps;
	}
}
