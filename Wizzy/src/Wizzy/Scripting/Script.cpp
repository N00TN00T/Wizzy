#include "wzpch.h"

#include "Wizzy/Scripting/Script.h"
#include "Wizzy/Scripting/ScriptManager.h"
#include "Wizzy/Utils.h"

namespace Wizzy {

	PropertyLibrary* Script::s_templateProps;

    Script::Script(const ResData& data, const PropertyLibrary& props)
        : Resource(props) 
	{
		m_sourceCode = (const char*)data.data();

		/*if (has_wz_meta(_data)) {
			string _rawMetaData = extract_wz_meta(_data, m_sourceCode);
			auto _metaData = parse_wz_meta(_rawMetaData);
			for (const auto& [_token, _value] : _metaData) {
				if (_token == WZ_META_TOKEN_CUSTOM) {
					m_scriptType = StringToScriptEnum(_value);
				}
			}
		} else if (flags.GetBit(SCRIPT_TYPE_SYSTEM)) {
			m_scriptType = SCRIPT_TYPE_SYSTEM;
		} else if (flags.GetBit(SCRIPT_TYPE_COMPONENT)) {
			m_scriptType = SCRIPT_TYPE_COMPONENT;
		} else if (flags.GetBit(SCRIPT_TYPE_CONFIG)) {
			m_scriptType = SCRIPT_TYPE_CONFIG;
		} else {
			WZ_CORE_ASSERT(false, "Invalid/Unimplemented script type");
		}*/

        //m_isValid = Do();

   //     if (m_isValid) {
			///*auto _ref2 = Get("v2");
			//auto _ref3 = Get("v3");
			//auto _ref4 = Get("v4");
			//auto _refT = Get("test");

			//auto L = ScriptManager::GetState(m_scriptType);
			//
			//vec2 _v2 =_ref2.cast<vec2>();
			//vec3 _v3 = _ref3.cast<vec3>();
			//vec4 _v4 = _ref4.cast<vec4>();
			//string _t = _refT.cast<string>();
			//
			//WZ_CORE_DEBUG("v2: {0}, {1}", _v2.x, _v2.y);
			//WZ_CORE_DEBUG("v3: {0}, {1}, {2}", _v3.x, _v3.y, _v3.z);
			//WZ_CORE_DEBUG("v4: {0}, {1}, {2}, {3}", _v4.x, _v4.y, _v4.z, _v4.w);
			//WZ_CORE_DEBUG(_t);*/
   //         WZ_CORE_INFO("Initialized a script");
   //     }
    }
	const PropertyLibrary& Script::GetTemplateProps()
	{
		if (!s_templateProps)
		{
			s_templateProps = new PropertyLibrary;
			
		}
		return *s_templateProps;
	}
	bool Script::Do() {
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
	}
	luabridge::LuaRef Script::Get(const string& name) {
		return luabridge::getGlobal(ScriptManager::GetState(m_scriptType), name.c_str());
	}
}
