#include "wzpch.h"

#include "Wizzy/LuaContext.h"
#include "Wizzy/Resources/Script.h"

namespace Wizzy {
    
    /* (lua_State *L) { 
            
            WZ_CORE_ASSERT(lua_gettop(L) >= 1, "Incorrect amount of arguments");
            WZ_CORE_ASSERT(lua_isuserdata(L, -1));
            void* _userData = lua_touserdata(L, -1);
            lua_getuservalue(L, -1);
            WZ_CORE_ASSERT(!lua_isnil(L, -1), "Target argument for Color.Magnitude is nil");
            lua_getfield(L, -1, "_typestr_");
            WZ_CORE_ASSERT(lua_isstring(L, -1) && lua_tostring(L, -1) == typestr(Color), "Invalid type of target argument given to Color.Magnitude()");
            
            
            
            Color& _color = *static_cast<Color*>(_userData);
            
            float _magnitude = std::sqrt(_color.r * _color.r + _color.r * _color.r + _color.r * _color.r + _color.r * _color.r);
            
            lua_pushnumber(_magnitude);
            
            return 1; 
        } */
    
    LuaContext::LuaContext() {
        Init();
    }
    
    void LuaContext::DoScript(Script* script) {
        WZ_CORE_ASSERT(script->GetScriptType() == SCRIPT_LUA, "Tried doing a non-lua script in a lua context");
        try {
            bool _error = luaL_dostring(m_L, script->GetScriptCode().c_str());
            if (_error) {
                WZ_CORE_ERROR("Lua error: {0}", lua_tostring(m_L, -1));
            }
        } catch (std::exception& e) {
            WZ_CORE_ERROR("LUA ERROR: '{0}'", e.what());
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
        LuaType<vec2> _vec2("Vec2");
        
        _vec2.Property<float>("x");
        _vec2.Property<float>("y");
        
        _vec2.Bind(m_L);
        
        LuaType<vec3> _vec3("Vec3");
        
        _vec3.Property<float>("x");
        _vec3.Property<float>("y");
        _vec3.Property<float>("z");
        
        _vec3.Bind(m_L);
        
        LuaType<vec4> _vec4("Vec4");
        
        _vec4.Property<float>("x");
        _vec4.Property<float>("y");
        _vec4.Property<float>("z");
        _vec4.Property<float>("w");
        
        _vec4.Bind(m_L);
        
        LuaType<Color> _color("Color");
        
        _color.Property<float>("r");
        _color.Property<float>("g");
        _color.Property<float>("b");
        _color.Property<float>("a");
        lua_CFunction _lerp = [](lua_State *L) {
            
            /*lua_getuservalue(L, -3);
            lua_getuservalue(L, -3);
            
            lua_getfield(L, -3, "_typestr_");
            lua_getfield(L, -3, "_typestr_");
            
            WZ_LUA_ASSERT((lua_isstring(L, -3) && lua_tostring(L, -3) == typestr(Color)));*/
            
            Color& _a = *static_cast<Color*>(lua_touserdata(L, -3));
            Color& _b = *static_cast<Color*>(lua_touserdata(L, -2));
            float _f = lua_tonumber(L, -1);
            
            lua_getglobal(L, "new");
            lua_getglobal(L, "Color");
            
            int32 _result = lua_pcall(L, 1, 1, 0);
            
            WZ_CORE_ASSERT(_result == LUA_OK, "Oopser doopser: " + string(lua_tostring(L, -1)));
            
            Color& _lerped = *static_cast<Color*>(lua_touserdata(L, -1));
            
            _lerped = Color::Lerp(_a, _b, _f);
            
            return 1;
        };
        _color.Method("Lerp", _lerp);
        _color.Bind(m_L);
    }
}