#pragma once

#include "Wizzy/Scripting/Script.h"

namespace Wizzy
{
	class LuaException : public std::exception
	{

	};

	class LuaBadArgVException : public LuaException
	{
	public:
		LuaBadArgVException(string reason, string func)
			: msg("Bad argument value (" + reason +") in function " + func)
		{

		}

		virtual char const* what() const override
		{
			return msg.c_str();
		}

	private:
		string msg;
	};

	class LuaBadArgCException : public LuaException
	{
	public:
		LuaBadArgCException(string goodCount, string actualCount, string func)
			: msg("Bad argument count (expected " + goodCount + ", got " + actualCount + ") in function " + func)
		{
			
		}

		virtual char const* what() const override
		{
			return msg.c_str();
		}

	private:
		string msg;
	};

	class LuaBadArgTException : public LuaException
	{
	public:
		LuaBadArgTException(string goodType, string actualType, string func)
			: msg("Bad argument type (expected " + goodType + ", got " + actualType + ") in function " + func)
		{

		}

		virtual char const* what() const override
		{
			return msg.c_str();
		}

	private:
		string msg;
	};

	class LuaIllegalWriteException : public LuaException
	{
	public:
		LuaIllegalWriteException(string prop)
			: msg("Cannot write to property '" + prop + "', it is read-only.")
		{

		}

		virtual char const* what() const override
		{
			return msg.c_str();
		}

	private:
		string msg;
	};

	class ScriptContext
	{
	public:
		ScriptContext();
		~ScriptContext();

		bool InvokeScript(Script::Handle hScript);

		bool IsFunction(int32 index);
		bool IsInteger(int32 index);
		bool IsNumber(int32 index);
		bool IsBoolean(int32 index);
		bool IsString(int32 index);
		bool IsTable(int32 index);
		template <typename T>
		bool Is(int32 index);

		//luabridge::LuaRef Get(string key);
		bool IsFunction(const string& key);
		bool IsInteger(const string& key);
		bool IsNumber(const string& key);
		bool IsBoolean(const string& key);
		bool IsString(const string& key);
		bool IsTable(const string& key);
		template <typename T>
		bool Is(const string& key);

		template <typename ...TArg>
		bool InvokeLuaFunction(int32 index, TArg... args);
		int32 GetInteger(int32 index) const;
		double GetNumber(int32 index) const;
		bool GetBoolean(int32 index) const;
		string GetString(int32 index) const;
		PropertyTable GetTable(int32 index);
		template <typename T>
		T Get(int32 index);

		template <typename ...TArg>
		bool InvokeLuaFunction(const string& name, TArg... args);
		int32 GetInteger(const string& key) const;
		double GetNumber(const string& key) const;
		bool GetBoolean(const string& key) const;
		string GetString(const string& key) const;
		PropertyTable GetTable(const string& key);
		template <typename T> 
		T Get(const string& key);

		void PushFunction(lua_CFunction value);
		void PushInteger(int32 value);
		void PushNumber(float value);
		void PushBoolean(bool value);
		void PushString(const string& value);
		void PushNewTable(const PropertyTable& value);
		template <typename T> 
		void Push(T value);

		void PushCopy(int32 index);
		void PushCopy(const string& key);

		void Pop(int32 index);

		void SetFunction(const string& key, lua_CFunction value);
		void SetInteger(const string& key, int32 value);
		void SetNumber(const string& key, float value);
		void SetBoolean(const string& key, bool value);
		void SetString(const string& key, const char* value);
		void SetTable(const string& key, const PropertyTable& table);

		// TODO: table functions with index
		void SetTableFunction(const string& tableKey, const string& fnName, lua_CFunction fn);
		void InvokeTableFunction(const string& tableKey, const string& fnName);
		void MakeTableReference(const string& src, const string& ref);

		void SetNil(const string& key);

		string GetTypeName(int32 index);
		string GetTypeName(const string& key);

		int32 GetStackCount();

	private:
		string TypeStrToScriptTypeName(const string& typeStr);
		PropertyTable ToWizzyTable(int32 luaStackIndex);

	private:
		lua_State* L;
	};
	template<typename T>
	inline bool ScriptContext::Is(const string& key)
	{
		if (IS_INT(T))								return IsInteger(key);
		if (IS_DECIMAL(T))							return IsNumber(key);
		if (IS_STRING(T))							return IsString(key);
		if (IS_BOOL(T))								return IsBoolean(key);
		if (typestr(T) == typestr(PropertyTable))	return IsTable(key);

		luabridge::LuaRef ref = luabridge::getGlobal(L, key.c_str());
		
		if (!ref.isNil() && ref.isUserdata())
		{
			WZ_CORE_ASSERT(lua_isuserdata(L, -1), "Registered Lua type is not userdata");
			lua_getuservalue(L, -1);
			lua_pushstring(L, "___T_");
			lua_gettable(L, -2);

			bool is = false;

			if (lua_isstring(L, -1))
			{
				is = lua_tostring(L, -1) == TypeStrToScriptTypeName(typestr(vec2));
			}

			lua_pop(L, -1);
			return is;
		}
	}
	template<typename ...TArg>
	inline bool ScriptContext::InvokeLuaFunction(int32 index, TArg ...args)
	{
		WZ_CORE_ASSERT(lua_isfunction(L, -1), "Invalid context function call");

		constexpr std::size_t argc = sizeof...(TArg);

		((void)Push(args), ...);

		auto result = lua_pcall(L, argc, 0, 0);

		if (result != LUA_OK)
		{
			WZ_CORE_ERROR("Error in function {0}", lua_tostring(L, -1));
			return false;
		}
		return true;
	}
	template<typename ...TArg>
	inline bool ScriptContext::InvokeLuaFunction(const string& name, TArg ...args)
	{
		lua_getglobal(L, name.c_str());
		
		return InvokeLuaFunction(-1, args...);
	}
	template<typename T>
	inline bool ScriptContext::Is(int32 index)
	{
		if (IS_INT(T)) { return IsInteger(index); }
		if (IS_DECIMAL(T)) { return IsNumber(index); }
		if (IS_STRING(T)) { return IsString(index); }
		if (IS_BOOL(T)) { return IsBoolean(index); }
		if (typestr(T) == typestr(PropertyTable)) { return IsTable(index); }

		return false;
	}
	template<typename T>
	inline T ScriptContext::Get(int32 index)
	{
		if (IS_INT(T)) { auto v = GetInteger(index); return *(T*)&v; }
		if (IS_DECIMAL(T)) { auto v = GetNumber(index); return *(T*)&v; }
		if (IS_STRING(T)) { auto v = GetString(index); return *(T*)&v; }
		if (IS_BOOL(T)) { auto v = GetBoolean(index); return *(T*)&v; }
		if (typestr(T) == typestr(PropertyTable)) { auto v = GetTable(index); return *(T*)&v; }

		WZ_CORE_ASSERT(false, "Invalid Get() call");
		int d;
		return *(T*)&d;
	}
	
	template<typename T>
	inline T ScriptContext::Get(const string& key)
	{
		if (IS_INT(T))								{ auto v = GetInteger(key); return *(T*)&v; }
		if (IS_DECIMAL(T))							{ auto v = GetNumber(key); return *(T*)&v; }
		if (IS_STRING(T))							{ auto v = GetString(key); return *(T*)&v; }
		if (IS_BOOL(T))								{ auto v = GetBoolean(key); return *(T*)&v; }
		if (typestr(T) == typestr(PropertyTable))	{ auto v = GetTable(key); return *(T*)&v; }

		luabridge::LuaRef ref = luabridge::getGlobal(L, key.c_str());

		WZ_CORE_ASSERT(!ref.isNil() && ref.isUserdata(), "Invalid Get() in script context");
		WZ_CORE_ASSERT(Is<T>(key), "Type mismatch in script context Get()");

		return ref.cast<T>();
	}
	template<typename T>
	inline void ScriptContext::Push(T value)
	{
		if constexpr (std::is_integral<T>() && !std::is_same<T, bool>())
		{
			return PushInteger(value);
		}
		else if constexpr (std::is_floating_point<T>())
		{
			return PushNumber(value);
		}
		else if constexpr (std::is_same<T, bool>())
		{
			return PushBoolean(value);
		}
		else if constexpr (std::is_same<T, string>() || std::is_same<T, char*>() || is_same<T, const char*>())
		{
			return PushString(value);
		}
	}
}