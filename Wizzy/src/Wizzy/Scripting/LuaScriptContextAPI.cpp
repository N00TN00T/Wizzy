#include "wzpch.h"

#include "Wizzy/Scripting/LuaScriptContextAPI.h"
#include "Wizzy/Scripting/Script.h"


namespace Wizzy 
{
    LuaScriptContextAPI::LuaScriptContextAPI()
    {
        L = luaL_newstate();
    }
    LuaScriptContextAPI::~LuaScriptContextAPI()
    {
        lua_close(L);
    }
    
    bool LuaScriptContextAPI::SupportsMultiReturn() const 
    {
        return true;
    }
    ScriptLang LuaScriptContextAPI::GetContextLang() const 
    {
        return script_lang_lua;
    }

    ScriptDataType LuaScriptContextAPI::GetType(const string& key) const 
    {
        PushGlobal(key);

        auto type = GetTypeIndexed(-1);

        Pop(1);

        return type;
    }

    void LuaScriptContextAPI::SetInt(const string& key, const ScriptInt& v) 
    {
        Push(v);
        lua_setglobal(L, key.c_str());
    }
    void LuaScriptContextAPI::SetFloat(const string& key, const ScriptFloat& v) 
    {
        Push(v);
        lua_setglobal(L, key.c_str());
    }
    void LuaScriptContextAPI::SetString(const string& key, const ScriptString& v) 
    {
        Push(v);
        lua_setglobal(L, key.c_str());
    }
    void LuaScriptContextAPI::SetBool(const string& key, ScriptBool v) 
    {
        Push(v);
        lua_setglobal(L, key.c_str());
    }
    void LuaScriptContextAPI::SetObject(const string& key, const ScriptObject& v) 
    {
        Push(v);
        lua_setglobal(L, key.c_str());
    }
    void LuaScriptContextAPI::SetObjectRef(const string& key, ScriptObjectRef v) 
    {
        Push(v);
        lua_setglobal(L, key.c_str());
    }
    void LuaScriptContextAPI::SetFunction(const string& key, ScriptCFunction v) 
    {
        Push(v);
        lua_setglobal(L, key.c_str());
    }
    void LuaScriptContextAPI::SetNull(const string& key)
    {
        PushNull();
        lua_setglobal(L, key.c_str());
    }

    ScriptInt LuaScriptContextAPI::GetInt(const string& key) const
    {
        PushGlobal(key);
        WZ_CORE_ASSERT(IsIndexInt(-1), "Type mismatch in ScriptContextAPI::Get()");
        ScriptInt ret = GetIntIndexed(-1);
        Pop(1);
        return ret;
    }
    ScriptFloat LuaScriptContextAPI::GetFloat(const string& key) const
    {
        PushGlobal(key);
        WZ_CORE_ASSERT(IsIndexFloat(-1), "Type mismatch in ScriptContextAPI::Get()");
        ScriptFloat ret = GetFloatIndexed(-1);
        Pop(1);
        return ret;
    }
    ScriptString LuaScriptContextAPI::GetString(const string& key) const
    {
        PushGlobal(key);
        WZ_CORE_ASSERT(IsIndexString(-1), "Type mismatch in ScriptContextAPI::Get()");
        ScriptString ret = GetStringIndexed(-1);
        Pop(1);
        return ret;
    }
    ScriptBool LuaScriptContextAPI::GetBool(const string& key) const
    {
        PushGlobal(key);
        WZ_CORE_ASSERT(IsIndexBool(-1), "Type mismatch in ScriptContextAPI::Get()");
        ScriptBool ret = GetBoolIndexed(-1);
        Pop(1);
        return ret;
    }
    ScriptObject LuaScriptContextAPI::GetObject(const string& key) const
    {
        PushGlobal(key);
        WZ_CORE_ASSERT(IsIndexObject(-1), "Type mismatch in ScriptContextAPI::Get()");
        ScriptObject ret = GetObjectIndexed(-1);
        Pop(1);
        return ret;
    }
    ScriptObjectRef LuaScriptContextAPI::GetObjectRef(const string& key) const
    {
        PushGlobal(key);
        WZ_CORE_ASSERT(IsIndexObjectRef(-1), "Type mismatch in ScriptContextAPI::Get()");
        ScriptObjectRef ret = GetObjectRefIndexed(-1);
        Pop(1);
        return ret;
    }
    ScriptFunction LuaScriptContextAPI::GetFunction(const string& key) const
    {
        PushGlobal(key);
        WZ_CORE_ASSERT(IsIndexFunction(-1), "Type mismatch in ScriptContextAPI::Get()");
        ScriptFunction ret = GetFunctionIndexed(-1);
        Pop(1);
        return ret;
    }

    bool LuaScriptContextAPI::IsInt(const string& key) const
    {
        PushGlobal(key);
        bool ret = IsIndexInt(-1);
        Pop(1);
        return ret;
    }
    bool LuaScriptContextAPI::IsFloat(const string& key) const
    {
        PushGlobal(key);
        bool ret = IsIndexFloat(-1);
        Pop(1);
        return ret;
    }
    bool LuaScriptContextAPI::IsString(const string& key) const
    {
        PushGlobal(key);
        bool ret = IsIndexString(-1);
        Pop(1);
        return ret;
    }
    bool LuaScriptContextAPI::IsBool(const string& key) const
    {
        PushGlobal(key);
        bool ret = IsIndexBool(-1);
        Pop(1);
        return ret;
    }
    bool LuaScriptContextAPI::IsObject(const string& key) const
    {
        PushGlobal(key);
        bool ret = IsIndexObject(-1);
        Pop(1);
        return ret;
    }
    bool LuaScriptContextAPI::IsObjectRef(const string& key) const
    {
        PushGlobal(key);
        bool ret = IsIndexObjectRef(-1);
        Pop(1);
        return ret;
    }
    bool LuaScriptContextAPI::IsFunction(const string& key) const
    {
        PushGlobal(key);
        bool ret = IsIndexFunction(-1);
        Pop(1);
        return ret;
    }
    bool LuaScriptContextAPI::IsNull(const string& key) const
    {
        PushGlobal(key);
        bool ret = IsIndexNull(-1);
        Pop(1);
        return ret;
    }

    void LuaScriptContextAPI::InvokeFunctionFromStack(u16 narg, const string& fnId)
    {
        WZ_CORE_ASSERT(IsIndexFunction(-1 - narg), "Invalid ScriptContext function call");
        
        auto result = lua_pcall(L, narg, 1, 0);

        if (result != LUA_OK)
        {
            string err = GetStringIndexed(-1);
            Pop(1);
            throw ScriptRuntimeException(err, fnId);
        }
    }

    void LuaScriptContextAPI::Pop(s16 c) const
    {
        lua_pop(L, c);
    }

    ScriptDataType LuaScriptContextAPI::GetTypeIndexed(s32 idx) const 
    {
        ScriptDataType type;

        if (IsIndexInt(idx))            type = ScriptDataType::_int;
        else if (IsIndexFloat(idx))     type = ScriptDataType::_float;
        else if (IsIndexString(idx))    type = ScriptDataType::_string;
        else if (IsIndexBool(idx))      type = ScriptDataType::_bool;
        else if (IsIndexObject(idx))    type = ScriptDataType::_object;
        else if (IsIndexObjectRef(idx)) type = ScriptDataType::_objectRef;
        else if (IsIndexFunction(idx))  type = ScriptDataType::_function;
        else if (IsIndexNull(idx))      type = ScriptDataType::_null;
        else 
        {
            WZ_CORE_CRITICAL("Unhandled lua type!");
            WZ_BREAK;
        }
        return type;
    }

    void LuaScriptContextAPI::PushGlobal(const string& key) const 
    {
        lua_getglobal(L, key.c_str());
    }
    void LuaScriptContextAPI::PushIndex(s16 idx) const 
    {
        lua_pushvalue(L, idx);
    }
    s16 LuaScriptContextAPI::GetTop() const 
    {
        return lua_gettop(L);
    }

    void LuaScriptContextAPI::PushInt(const ScriptInt& v) const
    {
        lua_pushinteger(L, v);
    }
    void LuaScriptContextAPI::PushFloat(const ScriptFloat& v) const
    {
        lua_pushnumber(L, v);
    }
    void LuaScriptContextAPI::PushString(const ScriptString& v) const
    {
        lua_pushstring(L, v);
    }
    void LuaScriptContextAPI::PushBool(ScriptBool v) const
    {
        lua_pushboolean(L, v);
    }
    void LuaScriptContextAPI::PushObject(const ScriptObject& v) const
    {
        lua_pushinteger(L, v);
    }
    void LuaScriptContextAPI::PushObjectRef(ScriptObjectRef v) const
    {
        lua_pushinteger(L, v);
    }
    void LuaScriptContextAPI::PushFunction(void* fnHandle) const
    {
        lua_pushcfunction(L, (lua_CFunction)fnHandle);
    }
    void LuaScriptContextAPI::PushNull() const
    {
        lua_pushnil(L);
    }

    ScriptInt LuaScriptContextAPI::GetIntIndexed(s16 idx) const
    {

    }
    ScriptFloat LuaScriptContextAPI::GetFloatIndexed(s16 idx) const
    {

    }
    ScriptString LuaScriptContextAPI::GetStringIndexed(s16 idx) const
    {

    }
    ScriptBool LuaScriptContextAPI::GetBoolIndexed(s16 idx) const
    {

    }
    ScriptObject LuaScriptContextAPI::GetObjectIndexed(s16 idx) const
    {

    }
    ScriptObjectRef LuaScriptContextAPI::GetObjectRefIndexed(s16 idx) const 
    {

    }
    ScriptFunction LuaScriptContextAPI::GetFunctionIndexed(s16 idx) const
    {

    }

    bool LuaScriptContextAPI::IsIndexInt(s16 idx) const 
    {

    }
    bool LuaScriptContextAPI::IsIndexFloat(s16 idx) const 
    {

    }
    bool LuaScriptContextAPI::IsIndexString(s16 idx) const 
    {

    }
    bool LuaScriptContextAPI::IsIndexBool(s16 idx) const 
    {

    }
    bool LuaScriptContextAPI::IsIndexObject(s16 idx) const 
    {

    }
    bool LuaScriptContextAPI::IsIndexObjectRef(s16 idx) const 
    {

    }
    bool LuaScriptContextAPI::IsIndexFunction(s16 idx) const 
    {

    }
    bool LuaScriptContextAPI::IsIndexNull(s16 idx) const 
    {

    }
}