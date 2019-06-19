#pragma once

#include <lua.hpp>

namespace Wizzy {
    
    class Script;
    
    enum class LuaDataType {
        NIL = 0, INT = 1, DECIMAL = 2, STRING = 3, BOOL = 4, USER_DATA = 5
    };
    
    struct LuaVec2 { // temporary
        float x, y;
    };
    
    struct LuaMethod {
        string name;
        lua_CFunction fn;
        
        LuaMethod(const string& name, const lua_CFunction& fn)
            : name(name), fn(fn) {}
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
            if (IS_INT(TProp)) {
                m_integers.push_back(name);
            } else if (IS_DECIMAL(TProp)) {
                m_numbers.push_back(name);    
            } else if (IS_STRING(TProp)) {
                m_strings.push_back(name);
            } else if (IS_BOOL(TProp)) {
                m_booleans.push_back(name);
            } else {
                WZ_CORE_ERROR("{0}, {1}", typestr(TProp), typestr(float));
                WZ_CORE_ASSERT(false, "OOGA BOOGA >:(");
            }
        }
    
        inline void Method(const string& name, 
                           lua_CFunction& fn) {
            m_methods.push_back(LuaMethod(name, fn));
        }
    
        inline void Bind(lua_State *L) {
            
            auto _newFn = [](lua_State *L) {
                WZ_CORE_ASSERT(lua_gettop(L) == 1, "Expected 1 argument for new(), got " + std::to_string(lua_gettop(L)));
                WZ_CORE_ASSERT(lua_isuserdata(L, -1), "Incorrect argument given for new()");
                
                size_t _size  = s_typeSizeByPtr[reinterpret_cast<uintptr_t>(lua_touserdata(L, -1))];
                lua_newuserdata(L, _size); /* TODO: Save ptr somewhere to track memory in lua from C++ */
                int32 _newUserDataIdx = lua_gettop(L);
                lua_getuservalue(L, -1);    // Get the table from user data argument
                /* Create a copy of the table */
                lua_newtable(L);
                int32 _destValueIdx = lua_gettop(L);
                lua_pushnil(L);  /* first key */
                WZ_CORE_DEBUG(lua_next(L, _destValueIdx));
                while (lua_next(L, _destValueIdx) != 0) {
                    WZ_CORE_DEBUG("YUH1");
                    lua_pushvalue(L, -2); // make copies of the
                    lua_pushvalue(L, -2); // key-value pair
                    lua_settable(L, _destValueIdx); // Add keyvalue pair to destination and pop them
                    lua_remove(L, -1); // Pop the value and leave the key for lua_next
                }
                lua_pushvalue(L, _newUserDataIdx);
                lua_pushvalue(L, _destValueIdx);
                
                WZ_CORE_ASSERT(lua_istable(L, - 1) && lua_isuserdata(L, -2), "Copying userdata value failed");
                
                lua_setuservalue(L, -2);
                
                lua_getuservalue(L, -1);
                
                return 1;
            };
            
            lua_pushcfunction(L, _newFn);
            lua_setglobal(L, "new");
            
            void* _dataPtr = lua_newuserdata(L, sizeof(T));
            
            s_typeSizeByPtr[reinterpret_cast<uintptr_t>(_dataPtr)] = sizeof(T);
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
            
            for (const LuaMethod& _method : m_methods) {
                char _name[1024] = "";
                strcpy(_name, _method.name.c_str());
                lua_pushstring(L, _name);
                lua_pushcfunction(L, _method.fn);
                lua_settable(L, -3);
            }
            
            string _typeStr = typestr(T);
            lua_pushstring(L, "_typestr_");
            lua_pushstring(L, _typeStr.c_str());
            lua_settable(L, -3);
            
            /* Set master instance data to the table created above */
            lua_setuservalue(L, -2); 
            
            lua_setglobal(L, m_name);
            
        }
        
    private:
        std::vector<string>                             m_numbers, m_integers, m_booleans, m_strings;
        std::vector<LuaMethod>                          m_methods;
        
        char *                                          m_name;
        
        static std::unordered_map <string, size_t>      s_typeSizeByName;
        static std::unordered_map <uintptr_t, size_t>   s_typeSizeByPtr;
    };
    
    template <typename T>
    std::unordered_map<string, size_t>  LuaType<T>::s_typeSizeByName;
    template <typename T>
    std::unordered_map<uintptr_t, size_t>  LuaType<T>::s_typeSizeByPtr;
    
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