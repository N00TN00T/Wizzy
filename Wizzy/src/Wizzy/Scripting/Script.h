#pragma once

#include "Wizzy/Resource/Resource.h"

#define WZ_DEFAULT_SCRIPT_LANG script_lang_lua

namespace Wizzy 
{
    enum ScriptLang : u8
    {
        script_lang_lua = 0, 
        script_lang_squirrel, 
        script_lang_wren,

        script_lang_count
    };

    inline string ScriptLangToString(ScriptLang lang)
    {
        switch (lang)
        {
            case script_lang_lua:       return "lua";
            case script_lang_squirrel:  return "squirrel";
            case script_lang_wren:      return "wren";
            default:                    return "[INVALID SCRIPTTYPE]";
        }
    }

    inline ScriptLang StringToScriptLang(const string& str)
    {
        if (str == "lua")           return script_lang_lua;
        else if (str == "squirrel") return script_lang_squirrel;
        else if (str == "wren")     return script_lang_wren;
        else 
        {
            WZ_CORE_WARN("Script type {0} not handled, defaulting to {1}", 
                            str, ScriptLangToString(WZ_DEFAULT_SCRIPT_LANG));

            return WZ_DEFAULT_SCRIPT_LANG;
        }
    }

    class Script
        : public Resource 
    {
    public:
		__HANDLE_DEF;
        Script(const ResData& data, const PropertyTable& props);
        Script(const string& codeStr, const PropertyTable& props);
        ~Script() {}
        inline virtual ResData Serialize() const override {
			return m_resData;
        }

		static const PropertyTable& GetTemplateProps();

        inline const string& GetScriptCode() const { return m_scriptStr; }

        inline ScriptLang GetScriptLang() const
        {
            return m_scriptLang;
        }

    public:
        inline static Resource* Create(const ResData& data, const PropertyTable& props) 
        {
            return new Script(data, props);
        }

        inline static Resource* Create(const string& codeStr, const PropertyTable& props = PropertyTable()) 
        {
            return new Script(codeStr, props);
        }
        inline static bool IsFileBinary() { return false; }

    private:
        union
        {
            string  m_scriptStr;
            ResData m_resData;
        };
        ScriptLang m_scriptLang = WZ_DEFAULT_SCRIPT_LANG;
        
	private:
		static PropertyTable* s_templateProps;
	};
}
