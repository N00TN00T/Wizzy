#include "wzpch.h"

#include "Wizzy/Scripting/Script.h"
#include "Wizzy/Utils.h"

namespace Wizzy {

	PropertyTable* Script::s_templateProps;

    Script::Script(const ResData& data, const PropertyTable& props)
        : Resource(props) 
	{
		m_sourceCode = (const char*)data.data();
    }
	const PropertyTable& Script::GetTemplateProps()
	{
		if (!s_templateProps)
		{
			s_templateProps = new PropertyTable;
			
		}
		return *s_templateProps;
	}
	/*bool Script::Do() {
		try {
			auto L = ScriptManager::GetState(m_scriptType);
			auto _doResult = luaL_dostring(L, m_sourceCode.c_str());
			if (_doResult != LUA_OK) {
				WZ_CORE_ERROR("Failed doing lua script: {0}", lua_tostring(L, -1));
				lua_remove(L, -1);
			}

			return _doResult == LUA_OK;
		} catch (const LuaException& e) {
			WZ_CORE_ERROR("LUA EXCEPTION THROWN: '{0}'", e.what());
			return false;
		} catch (const luabridge::LuaException& e) {
			WZ_CORE_ERROR("[LuaBridge] LUA EXCEPTION THROWN: '{0}'", e.what());
			return false;
		}
	}*/
	/*luabridge::LuaRef Script::Get(const string& name) {
		return luabridge::getGlobal(ScriptManager::GetState(m_scriptType), name.c_str());
	}*/
}
