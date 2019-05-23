#pragma once

#include <lua.hpp>

namespace Wizzy {
    
    class Script;
    
    struct LuaVec2 {
        float x, y;
    };
    
    template <typename T>
    class LuaType {
    public:
    
        inline LuaType<T>(const string& name) {
            m_name = new char[name.size()];
            strcpy(m_name, name.c_str());
            s_typeSizeByName[name] = sizeof(T);
        }
    
        template <typename TProp>
        inline void Property(const string& name) {
            if (typestr(TProp) == typestr(u8)     ||    typestr(TProp) == typestr(int8)    ||
                typestr(TProp) == typestr(u16)    ||    typestr(TProp) == typestr(int16)   ||
                typestr(TProp) == typestr(u32)    ||    typestr(TProp) == typestr(int32)   ||
                typestr(TProp) == typestr(u64)    ||    typestr(TProp) == typestr(int64)    ) {
                m_integers.push_back(name);
            } else if (typestr(TProp) == typestr(float) || typestr(TProp) == typestr(double)) {
                m_numbers.push_back(name);    
            } else if (typestr(TProp) == typestr(string) || typestr(TProp) == typestr(const char *)) {
                m_strings.push_back(name);
            } else if (typestr(TProp) == typestr(bool)) {
                m_booleans.push_back(name);
            } else {
                WZ_CORE_ERROR("{0}, {1}", typestr(TProp), typestr(float));
                WZ_CORE_ASSERT(false, "OOGA BOOGA >:(");
            }
        }
    
    
        inline void Bind(lua_State *L) {
            
            auto _newFn = [](lua_State *L) {
                WZ_CORE_ASSERT(lua_gettop(L) == 1, "Expected 1 argument for new(), got " + std::to_string(lua_gettop(L)));
                WZ_CORE_ASSERT(lua_isuserdata(L, -1) || lua_isstring(L, -1), "Incorrect arguments given for new()");
                
                size_t _size = 0;
                
                if (lua_isstring(L, -1)) {
                    _size = s_typeSizeByName[string(lua_tostring(L, -1))];
                    lua_getglobal(L, lua_tostring(L, -1));
                } else if (lua_isuserdata(L, -1)) {
                    _size = s_typeSizeByPtr[reinterpret_cast<uintptr_t>(lua_touserdata(L, -1))];
                } else {
                    WZ_CORE_ASSERT(false, "yikes"); // Shouldn't really be possible to ever happen
                }                                   // considering the assertion above
                
                lua_getuservalue(L, -1);    // Get the table from user data argument
                s_test.push_back(new(lua_newuserdata(L, _size)) T);  // Create a new userdata
                lua_pushvalue(L, -2);       // Push a COPY of the table associated with user data argument
                lua_setuservalue(L, -2);    // Set the table associated with the new user data to the copy
                
                lua_getuservalue(L, -1);    // Leave the new value of the new user data at the top of the stack
                
                //lua_remove(L, )
                
                
                return 1;
            };
            
            lua_pushcfunction(L, _newFn);
            lua_setglobal(L, "new");
            
            WZ_CORE_ASSERT((string(m_name) + "MetaTable").size() < 128, "Name of lua type too long");
            char _metaTableName[128] = "";
            strcat(_metaTableName, m_name);
            strcat(_metaTableName, "MetaTable");
            
            /*luaL_newmetatable(L, _metaTableName);
            
            lua_pushstring(LuaState::Get(), "__index");
			lua_pushcfunction(LuaState::Get(), [](lua_State *L) {
                
            });
            lua_settable(LuaState::Get(), -3);
            
            lua_pushstring(LuaState::Get(), "__newindex");
			lua_pushcfunction(LuaState::Get(), [](lua_State *L) {
                
            });
            lua_settable(LuaState::Get(), -3);*/
            
            m_masterInstance = new(lua_newuserdata(L, sizeof(T))) T;
            
            /*luaL_getmetatable(L, _metaTableName);
            lua_setmetatable(L, -2);*/
            
            s_typeSizeByPtr[reinterpret_cast<uintptr_t>(m_masterInstance)] = sizeof(T);
            lua_newtable(L);
            
            for (const auto& _name : m_numbers) {
                lua_pushstring(L, _name.c_str());
                lua_pushnumber(L, 0);
                lua_settable(L, -3);
            }
            
            for (const auto& _name : m_integers) {
                lua_pushstring(L, _name.c_str());
                lua_pushinteger(L, 0);
                lua_settable(L, -3);
            }
            
            for (const auto& _name : m_booleans) {
                lua_pushstring(L, _name.c_str());
                lua_pushboolean(L, false);
                lua_settable(L, -3);
            }
            
            for (const auto& _name : m_strings) {
                lua_pushstring(L, _name.c_str());
                lua_pushstring(L, "");
                lua_settable(L, -3);
            }
            
            /* Set master instance data to the table created above */
            lua_setuservalue(L, -2); 
            
            lua_setglobal(L, m_name);
            
        }
    private:
        std::vector<string>                         m_numbers, m_integers, m_booleans, m_strings;
        
        T*                                          m_masterInstance;
        
        char *                                      m_name;
        
        static std::unordered_map <string, size_t>  s_typeSizeByName;
        static std::unordered_map <uintptr_t, size_t>  s_typeSizeByPtr;
        
    public:
        static std::vector <T*> s_test;
    };
    
    template <typename T>
    std::unordered_map<string, size_t>  LuaType<T>::s_typeSizeByName;
    template <typename T>
    std::unordered_map<uintptr_t, size_t>  LuaType<T>::s_typeSizeByPtr;
    template <typename T>
    std::vector <T*> LuaType<T>::s_test;
    
    class LuaContext {
    public:
        LuaContext();
        
        void DoScript(Script* script);
        
        void Reset();
        
        void Close();
        
        int64 GetInteger(const string& name);
        string GetString(const string& name);
        double GetNumber(const string& name);
        bool GetBoolean(const string& name);
        
    private:
        void Init();
        void ExposeWizzy();
        
    private:
        lua_State *m_L;
    };
}