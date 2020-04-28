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
			WZ_CORE_ERROR("LUA EXCEPTION THROWN: '{0}'", e.GetMessage());
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

	bool ScriptContext::InvokeLuaFunctionStackArgs(int32 index, std::vector<int32> stackArgs, int32 nret)
	{
		PushCopy(index);
		int32 offset = 1;
		for (auto argIdx : stackArgs)
		{
			if (argIdx < 0) argIdx -= offset;
			PushCopy(argIdx);
			offset++;
		}
		bool result = InvokeFromStack(stackArgs.size(), nret);
		Pop(stackArgs.size());

		return result;
	}

	int32 ScriptContext::GetInteger(int32 index) const
	{
		WZ_CORE_ASSERT(lua_isinteger(L, index), "Invalid GetInteger() call");
		return lua_tointeger(L, index);
	}

	float ScriptContext::GetNumber(int32 index) const
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

	bool ScriptContext::InvokeLuaFunctionStackArgs(const string& name, std::vector<int32> stackArgs, int32 nret)
	{
		lua_getglobal(L, name.c_str());
		bool success = InvokeLuaFunctionStackArgs(-1, stackArgs, nret);
		Pop(1);
		return success;
	}

	int32 ScriptContext::GetInteger(const string& key) const
	{
		lua_getglobal(L, key.c_str());
		return GetInteger(-1);
	}

	float ScriptContext::GetNumber(const string& key) const
	{
		lua_getglobal(L, key.c_str());
		auto r = GetNumber(-1);
		lua_pop(L, 1);
		return r;
	}

	bool ScriptContext::GetBoolean(const string& key) const
	{
		lua_getglobal(L, key.c_str());
		auto r = GetBoolean(-1);
		lua_pop(L, 1);
		return r;
	}

	string ScriptContext::GetString(const string& key) const
	{
		lua_getglobal(L, key.c_str());
		auto r = GetString(-1);
		lua_pop(L, 1);
		return r;
	}

	PropertyTable ScriptContext::GetTable(const string& key)
	{
		lua_getglobal(L, key.c_str());
		auto r = GetTable(-1);
		lua_pop(L, 1);
		return r;
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
			if (table.Is<float>(propKey))			lua_pushnumber(L, table.Get<float>(propKey));
			if (table.Is<int32>(propKey))			lua_pushinteger(L, table.Get<int32>(propKey));
			if (table.Is<bool>(propKey))			lua_pushboolean(L, table.Get<bool>(propKey));
			if (table.Is<string>(propKey))			lua_pushstring(L, table.Get<string>(propKey).c_str());
			if (table.Is<PropertyTable>(propKey))	PushNewTable(table.Get<PropertyTable>(propKey));
			lua_setfield(L, -2, propKey.c_str());
		}
	}


#define _SET_VALUE(pushfn) \
pushfn(value);\
lua_setglobal(L, key.c_str());

	void ScriptContext::PushNil()
	{
		lua_pushnil(L);
	}

	void ScriptContext::PushCopy(int32 index)
	{
		lua_pushvalue(L, index);
	}

	void ScriptContext::PushCopy(const string& key)
	{
		lua_getglobal(L, key.c_str());
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

	void ScriptContext::SetFieldInteger(int32 tableIndex, const string& fieldName, int32 value)
	{
		WZ_CORE_ASSERT(lua_istable(L, tableIndex), "Invalid SetFieldXXX() call");
		lua_pushinteger(L, value);
		if (tableIndex < 0) tableIndex--;
		lua_setfield(L, tableIndex, fieldName.c_str());
	}

	void ScriptContext::SetFieldNumber(int32 tableIndex, const string& fieldName, float value)
	{
		WZ_CORE_ASSERT(lua_istable(L, tableIndex), "Invalid SetFieldXXX() call");
		lua_pushnumber(L, value);
		if (tableIndex < 0) tableIndex--;
		lua_setfield(L, tableIndex, fieldName.c_str());
	}

	void ScriptContext::SetFieldBoolean(int32 tableIndex, const string& fieldName, bool value)
	{
		WZ_CORE_ASSERT(lua_istable(L, tableIndex), "Invalid SetFieldXXX() call");
		lua_pushboolean(L, value);
		if (tableIndex < 0) tableIndex--;
		lua_setfield(L, tableIndex, fieldName.c_str());
	}

	void ScriptContext::SetFieldString(int32 tableIndex, const string& fieldName, const char* value)
	{
		WZ_CORE_ASSERT(lua_istable(L, tableIndex), "Invalid SetFieldXXX() call");
		lua_pushstring(L, value);
		if (tableIndex < 0) tableIndex--;
		lua_setfield(L, tableIndex, fieldName.c_str());
	}

	void ScriptContext::SetFieldSubtable(int32 tableIndex, const string& fieldName, const PropertyTable& table)
	{
		WZ_CORE_ASSERT(lua_istable(L, tableIndex), "Invalid SetFieldXXX() call");
		PushNewTable(table);
		if (tableIndex < 0) tableIndex--;
		lua_setfield(L, tableIndex, fieldName.c_str());
	}

	void ScriptContext::SetFieldSubtable(int32 target, const string& fieldName, const string& luaTable)
	{
		WZ_CORE_ASSERT(lua_istable(L, target), "Invalid SetFieldXXX() call");
		lua_getglobal(L, luaTable.c_str());
		WZ_CORE_ASSERT(lua_istable(L, -1), "Invalid SetFieldXXX() call");
		if (target < 0) target--;
		lua_setfield(L, target, fieldName.c_str());
	}

	void ScriptContext::SetFieldSubtable(int32 target, const string& fieldName, int32 luaTable)
	{
		WZ_CORE_ASSERT(lua_istable(L, target) && lua_istable(L, luaTable), "Invalid SetFieldXXX() call");
		lua_pushvalue(L, luaTable);
		if (target < 0) target--;
		lua_setfield(L, target, fieldName.c_str());
		lua_remove(L, luaTable);
	}

	void ScriptContext::SetTableFunction(int32 tableIndex, const string& fnName, lua_CFunction fn)
	{
		WZ_CORE_ASSERT(lua_istable(L, tableIndex), "Invalid SetTableFunction() call");
		lua_pushcfunction(L, fn);
		if (tableIndex < 0) tableIndex--;
		lua_setfield(L, tableIndex, fnName.c_str());
	}

	bool ScriptContext::InvokeTableFunction(int32 tableIndex, const string& fnName)
	{
		WZ_CORE_ASSERT(IsTable(tableIndex), "Invalid InvokeTableFunction() call");
		lua_getfield(L, tableIndex, fnName.c_str());
		WZ_CORE_ASSERT(IsFunction(tableIndex), "Invalid InvokeTableFunction() call");

		return InvokeLuaFunctionStackArgs(tableIndex);
	}

	void ScriptContext::MakeTableReference(int32 tableIndex, const string& ref)
	{
		lua_pushvalue(L, tableIndex);
		lua_setglobal(L, ref.c_str());
	}


	void ScriptContext::SetFieldInteger(const string& key, const string& fieldName, int32 value)
	{
		lua_getglobal(L, key.c_str());
		SetFieldInteger(-1, fieldName, value);
		Pop(1);
	}

	void ScriptContext::SetFieldNumber(const string& key, const string& fieldName, float value)
	{
		lua_getglobal(L, key.c_str());
		SetFieldNumber(-1, fieldName, value);
		Pop(1);
	}

	void ScriptContext::SetFieldBoolean(const string& key, const string& fieldName, bool value)
	{
		lua_getglobal(L, key.c_str());
		SetFieldBoolean(-1, fieldName, value);
		Pop(1);
	}

	void ScriptContext::SetFieldString(const string& key, const string& fieldName, const char* value)
	{
		lua_getglobal(L, key.c_str());
		SetFieldString(-1, fieldName, value);
		Pop(1);
	}

	void ScriptContext::SetFieldSubtable(const string& key, const string& fieldName, const PropertyTable& table)
	{
		lua_getglobal(L, key.c_str());
		SetFieldSubtable(-1, fieldName, table);
		lua_setglobal(L, key.c_str());
		Pop(1);
	}

	void ScriptContext::SetFieldSubtable(const string& target, const string& fieldName, const string& luaTable)
	{
		lua_getglobal(L, target.c_str());
		SetFieldSubtable(-1, fieldName, luaTable);
		Pop(1);
	}

	void ScriptContext::SetFieldSubtable(const string& target, const string& fieldName, int32 luaTable)
	{
		lua_getglobal(L, target.c_str());
		if (luaTable < 0)luaTable--;
		SetFieldSubtable(-1, fieldName, luaTable);
		Pop(1);
	}

	void ScriptContext::SetTableFunction(const string& tableKey, const string& fnName, lua_CFunction fn)
	{
		lua_getglobal(L, tableKey.c_str());
		SetTableFunction(-1, fnName, fn);
		Pop(1);
	}

	bool ScriptContext::InvokeTableFunction(const string& tableKey, const string& fnName)
	{
		lua_getglobal(L, tableKey.c_str());
		bool result = InvokeTableFunction(-1, fnName);
		Pop(1);
		return result;
	}

	void ScriptContext::MakeTableReference(const string& src, const string& ref)
	{
		WZ_CORE_ASSERT(IsTable(src), "Invalid MakeTableReference() call");
		lua_getglobal(L, src.c_str());
		lua_setglobal(L, ref.c_str());
	}

	bool ScriptContext::IsFieldInteger(int32 table, const string& field)
	{
		WZ_CORE_ASSERT(lua_istable(L, table), "Invalid IsFieldXXXX call");
		lua_getfield(L, table, field.c_str());
		bool is = lua_isinteger(L, -1);
		lua_pop(L, 1);
		return is;
	}

	bool ScriptContext::IsFieldNumber(int32 table, const string& field)
	{
		WZ_CORE_ASSERT(lua_istable(L, table), "Invalid IsFieldXXXX call");
		lua_getfield(L, table, field.c_str());
		bool is = lua_isnumber(L, -1);
		lua_pop(L, 1);
		return is;
	}

	bool ScriptContext::IsFieldBoolean(int32 table, const string& field)
	{
		WZ_CORE_ASSERT(lua_istable(L, table), "Invalid IsFieldXXXX call");
		lua_getfield(L, table, field.c_str());
		bool is = lua_isboolean(L, -1);
		lua_pop(L, 1);
		return is;
	}

	bool ScriptContext::IsFieldString(int32 table, const string& field)
	{
		WZ_CORE_ASSERT(lua_istable(L, table), "Invalid IsFieldXXXX call");
		lua_getfield(L, table, field.c_str());
		bool is = lua_isstring(L, -1);
		lua_pop(L, 1);
		return is;
	}

	bool ScriptContext::IsFieldTable(int32 table, const string& field)
	{
		WZ_CORE_ASSERT(lua_istable(L, table), "Invalid IsFieldXXXX call");
		lua_getfield(L, table, field.c_str());
		bool is = lua_istable(L, -1);
		lua_pop(L, 1);
		return is;
	}

	bool ScriptContext::IsFieldFunction(int32 table, const string& field)
	{
		WZ_CORE_ASSERT(lua_istable(L, table), "Invalid IsFieldXXXX call");
		lua_getfield(L, table, field.c_str());
		bool is = lua_isfunction(L, -1);
		lua_pop(L, 1);
		return is;
	}

	bool ScriptContext::IsFieldInteger(const string& table, const string& field)
	{
		lua_getglobal(L, table.c_str());
		bool is = IsFieldInteger(-1, field);
		Pop(1);
		return is;
	}

	bool ScriptContext::IsFieldNumber(const string& table, const string& field)
	{
		lua_getglobal(L, table.c_str());
		bool is = IsFieldNumber(-1, field);
		Pop(1);
		return is;
	}

	bool ScriptContext::IsFieldBoolean(const string& table, const string& field)
	{
		lua_getglobal(L, table.c_str());
		bool is = IsFieldBoolean(-1, field);
		Pop(1);
		return is;
	}

	bool ScriptContext::IsFieldString(const string& table, const string& field)
	{
		lua_getglobal(L, table.c_str());
		bool is = IsFieldString(-1, field);
		Pop(1);
		return is;
	}

	bool ScriptContext::IsFieldTable(const string& table, const string& field)
	{
		lua_getglobal(L, table.c_str());
		bool is = IsFieldTable(-1, field);
		Pop(1);
		return is;
	}

	bool ScriptContext::IsFieldFunction(const string& table, const string& field)
	{
		lua_getglobal(L, table.c_str());
		bool is = IsFieldFunction(-1, field);
		Pop(1);
		return is;
	}

	void ScriptContext::CreateMetaTable(const string& metaTable)
	{
		luaL_newmetatable(L, metaTable.c_str());
		lua_pop(L, 1);
	}

	void ScriptContext::SetMetaMethod(const string& metaTable, ScriptOperator op, lua_CFunction fn)
	{
		luaL_getmetatable(L, metaTable.c_str());
		WZ_CORE_ASSERT(!lua_isnil(L, -1), "Invalid SetMetaMethod() call");
		lua_pushcfunction(L, fn);
		string ops = OperatorString(op);
		lua_setfield(L, -2, ops.c_str());
		lua_pop(L, 1);
	}

	void ScriptContext::SetMetaTable(int32 tableIndex, const string& metaTable)
	{
		WZ_CORE_ASSERT(lua_istable(L, tableIndex), "Invalid SetMetaTable() call");
		luaL_getmetatable(L, metaTable.c_str());
		if (tableIndex < 0) tableIndex--;
		WZ_CORE_ASSERT(lua_istable(L, -1), "Invalid SetMetaTable() call");
		lua_setmetatable(L, tableIndex);
	}

	void ScriptContext::SetMetaTable(const string& table, const string& metaTable)
	{
		lua_getglobal(L, table.c_str());
		SetMetaTable(-1, metaTable);
		Pop(1);
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

	size_t ScriptContext::GetMemoryUsage()
	{
		return lua_gc(L, LUA_GCCOUNT, 0);
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
				table.Set<int32>(key, lua_tointeger(L, -2));
			}
			else if (lua_isnumber(L, -2))
			{
				table.Set<float>(key, lua_tonumber(L, -2));
			}
			else if (lua_isboolean(L, -2))
			{
				table.Set<bool>(key, lua_toboolean(L, -2));
			}
			else if (lua_isstring(L, -2))
			{
				table.Set<string>(key, lua_tostring(L, -2));
			}
			else if (lua_istable(L, -2))
			{
				table.Set<PropertyTable>(key, ToWizzyTable(-2));
			}
			else if (lua_isuserdata(L, -2))
			{
				lua_getuservalue(L, -2);
				table.Set<PropertyTable>(key, ToWizzyTable(-1));
				lua_pop(L, -1);
			}

			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		return table;
	}

	string ScriptContext::OperatorString(ScriptOperator op)
	{
		const static std::unordered_map<ScriptOperator, string> opStr = 
		{
			{ script_operator_index,	"__index"		},
			{ script_operator_newindex,	"__newindex"	},
			{ script_operator_mode,		"__mode"		},
			{ script_operator_call,		"__call"		},
			{ script_operator_metatable,"__metatable"	},
			{ script_operator_tostring,	"__tostring"	},
			{ script_operator_len,		"__len"			},
			{ script_operator_pairs,	"__pairs"		},
			{ script_operator_ipairs,	"__ipairs"		},
			{ script_operator_gc,		"__gc"			},
			{ script_operator_unm,		"__unm"			},
			{ script_operator_add,		"__add"			},
			{ script_operator_sub,		"__sub"			},
			{ script_operator_mul,		"__mul"			},
			{ script_operator_div,		"__div"			},
			{ script_operator_idiv,		"__idiv"		},
			{ script_operator_mod,		"__mod"			},
			{ script_operator_pow,		"__pow"			},
			{ script_operator_concat,	"__concat"		},
			{ script_operator_band,		"__band"		},
			{ script_operator_bor,		"__bor"			},
			{ script_operator_bxor,		"__bxor"		},
			{ script_operator_bnot,		"__bnot"		},
			{ script_operator_shl,		"__shl"			},
			{ script_operator_shr,		"__shr"			},
			{ script_operator_eq,		"__eg"			},
			{ script_operator_lt,		"__lt"			},
			{ script_operator_le,		"__le"			}
		};

		WZ_CORE_ASSERT(opStr.find(op) != opStr.end(), "String not specified for script operator");

		return opStr.at(op);
	}
	bool ScriptContext::InvokeFromStack(int32 narg, int32 nret)
	{
		auto result = lua_pcall(L, narg, nret, 0);
		if (result != LUA_OK)
		{
			string msg = lua_isstring(L, -1) ? lua_tostring(L, -1) : "N/A";
			WZ_CORE_ERROR("Lua function error {0}: {1}", result, msg);
			return false;
		}
		return true;
	}
}