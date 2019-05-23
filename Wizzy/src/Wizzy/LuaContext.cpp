#include "wzpch.h"

#include "Wizzy/LuaContext.h"
#include "Wizzy/Resources/Script.h"

namespace Wizzy {
    
    LuaContext::LuaContext() {
        Init();
    }
    
    void LuaContext::DoScript(Script* script) {
        WZ_CORE_ASSERT(script->GetScriptType() == SCRIPT_LUA, "Tried doing a non-lua script in a lua context");
        bool _error = luaL_dostring(m_L, script->GetScriptCode().c_str());
        if (_error) {
            WZ_CORE_ERROR("Lua error: {0}", lua_tostring(m_L, -1));
        }
    }
    
    void LuaContext::Reset() {
        Close();
        Init();
    }
    
    void LuaContext::Close() {
        lua_close(m_L);
    }
    
    int64 LuaContext::GetInteger(const string& name) {
        lua_getglobal(m_L, name.c_str());
        return lua_tointeger(m_L, -1);
    }
    string LuaContext::GetString(const string& name) {
        lua_getglobal(m_L, name.c_str());
        return lua_tostring(m_L, -1);
    }
    double LuaContext::GetNumber(const string& name) {
        lua_getglobal(m_L, name.c_str());
        return lua_tonumber(m_L, -1);
    }
    bool LuaContext::GetBoolean(const string& name) {
        lua_getglobal(m_L, name.c_str());
        return lua_toboolean(m_L, -1);
    }
    
    void LuaContext::Init() {
        m_L = luaL_newstate();
        
        lua_pushcfunction(m_L, [](lua_State *L) { WZ_TRACE(lua_tostring(L, -1)); return 0; });
        lua_setglobal(m_L, "print");
        
        ExposeWizzy();
    }
    
    void LuaContext::ExposeWizzy() {
        LuaType<LuaVec2> _vec2("Vec2");
        
        _vec2.Property<float>("x");
        _vec2.Property<float>("y");
        
        _vec2.Bind(m_L);
    }
}