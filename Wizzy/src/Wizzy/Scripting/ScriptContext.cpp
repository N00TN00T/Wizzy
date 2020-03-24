#include "wzpch.h"

#include "Wizzy/Scripting/ScriptContext.h"
#include "Wizzy/Resource/ResourceManagement.h"

namespace Wizzy
{
	ScriptContext::ScriptContext()
	{
		L = luaL_newstate();
		luaL_openlibs(L);
	}
	ScriptContext::~ScriptContext()
	{
		lua_close(L);
	}
	bool ScriptContext::InvokeScript(Script::Handle hScript)
	{
		WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hScript), "Script has to be loaded tp be invoked");

		auto& script = ResourceManagement::Get<Script>(hScript);

		WZ_DEBUG("Doing script:");
		Log::SetExtra(false);
		WZ_DEBUG(script.GetScriptCode().c_str());
		Log::SetExtra(true);

		try
		{
			auto result = luaL_dostring(L, script.GetScriptCode().c_str());
			if (result != LUA_OK)
			{
				WZ_CORE_ERROR("Failed doing lua script: {0}", lua_tostring(L, -1));
				lua_remove(L, -1);
			}

			return result == LUA_OK;
		}
		catch (const LuaException & e)
		{
			WZ_CORE_ERROR("LUA EXCEPTION THROWN: '{0}'", e.what());
			return false;
		}
		catch (const luabridge::LuaException & e)
		{
			WZ_CORE_ERROR("[LuaBridge] LUA EXCEPTION THROWN: '{0}'", e.what());
			return false;
		}
	}

#define _CHECK_IS(checkFn)\
lua_getglobal(L, key.c_str());\
bool is = checkFn(-1);\
lua_pop(L, -1);\
return is;

	bool ScriptContext::IsFunction(int32 index)
	{
		return lua_isfunction(L, index);
	}

	bool ScriptContext::IsInteger(int32 index)
	{
		return lua_isinteger(L, index);
	}

	bool ScriptContext::IsNumber(int32 index)
	{
		return lua_isnumber(L, index);
	}

	bool ScriptContext::IsBoolean(int32 index)
	{
		return lua_isboolean(L, index);
	}

	bool ScriptContext::IsString(int32 index)
	{
		return lua_isstring(L, index);
	}

	bool ScriptContext::IsTable(int32 index)
	{
		return lua_istable(L, index);
	}

	bool ScriptContext::IsFunction(const string& key)
	{
		_CHECK_IS(IsFunction);
	}
	bool ScriptContext::IsInteger(const string& key)
	{
		_CHECK_IS(IsInteger);
	}
	bool ScriptContext::IsNumber(const string& key)
	{
		_CHECK_IS(IsNumber);
	}
	bool ScriptContext::IsBoolean(const string& key)
	{
		_CHECK_IS(IsBoolean);
	}
	bool ScriptContext::IsString(const string& key)
	{
		_CHECK_IS(IsString);
	}
	bool ScriptContext::IsTable(const string& key)
	{
		_CHECK_IS(IsTable);
	}

	int32 ScriptContext::GetInteger(int32 index) const
	{
		WZ_CORE_ASSERT(lua_isinteger(L, index), "Invalid GetInteger() call");
		return lua_tointeger(L, index);
	}

	double ScriptContext::GetNumber(int32 index) const
	{
		WZ_CORE_ASSERT(lua_isnumber(L, index), "Invalid GetNumber() call");
		return lua_tonumber(L, index);
	}

	bool ScriptContext::GetBoolean(int32 index) const
	{
		WZ_CORE_ASSERT(lua_isboolean(L, index), "Invalid GetBoolean() call");
		return lua_toboolean(L, index);
	}

	string ScriptContext::GetString(int32 index) const
	{
		WZ_CORE_ASSERT(lua_isstring(L, index), "Invalid GetString() call");
		return lua_tostring(L, index);
	}

	PropertyTable ScriptContext::GetTable(int32 index)
	{
		WZ_CORE_ASSERT(lua_istable(L, index), "Invalid GetXXXX() call");
		return ToWizzyTable(index);
	}

	int32 ScriptContext::GetInteger(const string& key) const
	{
		lua_getglobal(L, key.c_str());
		return GetInteger(-1);
	}

	double ScriptContext::GetNumber(const string& key) const
	{
		lua_getglobal(L, key.c_str());
		return GetNumber(-1);
	}

	bool ScriptContext::GetBoolean(const string& key) const
	{
		lua_getglobal(L, key.c_str());
		return GetBoolean(-1);
	}

	string ScriptContext::GetString(const string& key) const
	{
		lua_getglobal(L, key.c_str());
		return GetString(-1);
	}

	PropertyTable ScriptContext::GetTable(const string& key)
	{
		lua_getglobal(L, key.c_str());
		return GetTable(-1);
	}

	void ScriptContext::PushFunction(lua_CFunction value)
	{
		lua_pushcfunction(L, value);
	}

	void ScriptContext::PushInteger(int32 value)
	{
		lua_pushinteger(L, value);
	}

	void ScriptContext::PushNumber(float value)
	{
		lua_pushnumber(L, value);
	}

	void ScriptContext::PushBoolean(bool value)
	{
		lua_pushboolean(L, value);
	}

	void ScriptContext::PushString(const string& value)
	{
		lua_pushstring(L, value.c_str());
	}

	void ScriptContext::PushNewTable(const PropertyTable& table)
	{
		const auto& props = table.GetKeys();
		lua_newtable(L, 0, props.size());
		int tIndex = lua_gettop(L);

		for (const auto& propKey : props)
		{
			if (table.IsProperty<float>(propKey))			lua_pushnumber(L, table.GetProperty<float>(propKey));
			if (table.IsProperty<int32>(propKey))			lua_pushinteger(L, table.GetProperty<int32>(propKey));
			if (table.IsProperty<bool>(propKey))			lua_pushboolean(L, table.GetProperty<bool>(propKey));
			if (table.IsProperty<string>(propKey))			lua_pushstring(L, table.GetProperty<string>(propKey).c_str());
			if (table.IsProperty<PropertyTable>(propKey))	PushNewTable(table.GetProperty<PropertyTable>(propKey));
			lua_setfield(L, -2, propKey.c_str());
		}
	}


#define _SET_VALUE(pushfn) \
pushfn(value);\
lua_setglobal(L, key.c_str());

	void ScriptContext::PushCopy(int32 index)
	{
		lua_pushvalue(L, index);
	}

	void ScriptContext::PushCopy(const string& key)
	{
		lua_getglobal(L, key.c_str());
		PushCopy(-1);
	}

	void ScriptContext::Pop(int32 index)
	{
		lua_pop(L, index);
	}

	void ScriptContext::SetFunction(const string& key, lua_CFunction value)
	{
		_SET_VALUE(PushFunction);
	}

	void ScriptContext::SetInteger(const string& key, int32 value)
	{
		_SET_VALUE(PushInteger);
	}

	void ScriptContext::SetNumber(const string& key, float value)
	{
		_SET_VALUE(PushNumber);
	}

	void ScriptContext::SetBoolean(const string& key, bool value)
	{
		_SET_VALUE(PushBoolean);
	}

	void ScriptContext::SetString(const string& key, const char* value)
	{
		_SET_VALUE(PushString);
	}

	void ScriptContext::SetTable(const string& key, const PropertyTable& table)
	{
		PushNewTable(table);
		lua_setglobal(L, key.c_str());
	}

	void ScriptContext::SetTableFunction(const string& tableKey, const string& fnName, lua_CFunction fn)
	{
		lua_getglobal(L, tableKey.c_str());
		WZ_CORE_ASSERT(lua_istable(L, -1), "Invalid SetTableFunction() call");
		lua_pushcfunction(L, fn);
		lua_setfield(L, -2, fnName.c_str());
	}

	void ScriptContext::InvokeTableFunction(const string& tableKey, const string& fnName)
	{
		lua_getglobal(L, tableKey.c_str());
		WZ_CORE_ASSERT(IsTable(-1), "Invalid InvokeTableFunction() call");
		lua_getfield(L, -1, fnName.c_str());
		WZ_CORE_ASSERT(IsFunction(-1), "Invalid InvokeTableFunction() call");

		InvokeLuaFunction(-1);
	}

	void ScriptContext::MakeTableReference(const string& src, const string& ref)
	{
		WZ_CORE_ASSERT(IsTable(src), "Invalid MakeTableReference() call");
		lua_getglobal(L, src.c_str());
		lua_pushvalue(L, -1);
		lua_setglobal(L, ref.c_str());
		lua_pop(L, -1);
	}

	void ScriptContext::SetNil(const string& key)
	{
		lua_pushnil(L);
		lua_setglobal(L, key.c_str());
	}

	string ScriptContext::GetTypeName(int32 index)
	{
		return lua_typename(L, lua_type(L, index));;
	}

	string ScriptContext::GetTypeName(const string& key)
	{
		lua_getglobal(L, key.c_str());
		string tn = GetTypeName(-1);
		Pop(-1);
		return tn;
	}

	int32 ScriptContext::GetStackCount()
	{
		return lua_gettop(L);
	}
	

	string ScriptContext::TypeStrToScriptTypeName(const string& typeStr)
	{
		if (typeStr == typestr(vec2))	return "Vector2";
		if (typeStr == typestr(vec3))	return "Vector3";
		if (typeStr == typestr(vec4))	return "Vector4";
		if (typeStr == typestr(Color))	return "Color";

		return "N/A";
	}

	PropertyTable ScriptContext::ToWizzyTable(int32 luaStackIndex)
	{
		PropertyTable table;

		lua_pushvalue(L, luaStackIndex);
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			lua_pushvalue(L, -2);
			string key = lua_tostring(L, -1);
			//const char* value = lua_tostring(L, -2);

			if (lua_isinteger(L, -2))
			{
				table.SetProperty<int32>(key, lua_tointeger(L, -2));
			}
			else if (lua_isnumber(L, -2))
			{
				table.SetProperty<float>(key, lua_tonumber(L, -2));
			}
			else if (lua_isboolean(L, -2))
			{
				table.SetProperty<bool>(key, lua_toboolean(L, -2));
			}
			else if (lua_isstring(L, -2))
			{
				table.SetProperty<string>(key, lua_tostring(L, -2));
			}
			else if (lua_istable(L, -2))
			{
				table.SetProperty<PropertyTable>(key, ToWizzyTable(-2));
			}
			else if (lua_isuserdata(L, -2))
			{
				lua_getuservalue(L, -2);
				table.SetProperty<PropertyTable>(key, ToWizzyTable(-1));
				lua_pop(L, -1);
			}

			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		return table;
	}

	/*luabridge::LuaRef ScriptContext::Get(string key)
	{
		return luabridge::getGlobal(L, key.c_str());
	}*/
}