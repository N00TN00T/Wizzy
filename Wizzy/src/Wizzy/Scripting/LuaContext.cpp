#include "wzpch.h"

#include "Wizzy/Scripting/LuaContext.h"
#include "Wizzy/Scripting/Script.h"

namespace Wizzy {

    LuaContext::LuaContext() {
        Init();
    }
    LuaContext::~LuaContext() {
        if (IsValid()) Close();
    }

    void LuaContext::DoScript(Script* script) {
        WZ_CORE_ASSERT(script->GetScriptType() == SCRIPT_LUA, "Tried doing a non-lua script in a lua context");
        try {
            bool _error = luaL_dostring(m_L, script->GetScriptCode().c_str());
            if (_error) {
                WZ_CORE_ERROR("Lua error: {0}", lua_tostring(m_L, -1));
            }
        } catch (std::exception& e) {
            WZ_CORE_ERROR("LUA EXCEPTION: '{0}'", e.what());
        }

    }

    void LuaContext::Reset() {
        Close();
        Init();
    }

    void LuaContext::Close() {
        lua_close(m_L);
        m_L = nullptr;
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

        lua_pushcfunction(m_L, [](lua_State *L) { WZ_DEBUG("[LUA PRINT] {0}", lua_tostring(L, -1)); return 0; });
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
