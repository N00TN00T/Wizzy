#pragma once

#include "Wizzy/Resource/Resource.h"

namespace Wizzy 
{
    class Script
        : public Resource 
    {
    public:
		__HANDLE_DEF;
        Script(const ResData& data, const PropertyTable& props);

        inline virtual ResData Serialize() const override {
			return ResData((const byte*)m_sourceCode.data(), (const byte*)m_sourceCode.data() + m_sourceCode.length());
        }

		static const PropertyTable& GetTemplateProps();

		//luabridge::LuaRef Get(const string& name);

        inline const string&		GetScriptCode() const { return m_sourceCode; }

        inline static Resource* Create(const ResData& data, const PropertyTable& props) {
            return new Script(data, props);
        }

        inline static bool IsFileBinary() { return false; }

    private:
        string                  m_sourceCode = "";
	private:
		static PropertyTable* s_templateProps;
	};
}
