#pragma once

#include "Wizzy/Scripting/Script.h"
#include "Wizzy/WizzyExceptions.h"

namespace Wizzy
{
	class LuaException : public Exception
	{
	public:
		LuaException(string msg, const string& fn) : Exception(msg, -1, fn) {}
	};

	class LuaBadArgVException : public LuaException
	{
	public:
		LuaBadArgVException(string reason, const string& fn)
			: LuaException("Bad argument value (" + reason + ") in function " + fn, fn)
		{
			
		}
	};

	class LuaBadArgCException : public LuaException
	{
	public:
		LuaBadArgCException(string goodCount, string actualCount, const string& fn)
			: LuaException("Bad argument count (expected " + goodCount + ", got " + actualCount + ") in function " + fn, fn)
		{
			
		}
	};

	class LuaBadArgTException : public LuaException
	{
	public:
		LuaBadArgTException(string goodType, string actualType, string fn)
			: LuaException("Bad argument type (expected " + goodType + ", got " + actualType + ") in function " + fn, fn)
		{

		}
	};

	class LuaIllegalWriteException : public LuaException
	{
	public:
		LuaIllegalWriteException(const string& prop, const string& fn)
			: LuaException("Cannot write to property '" + prop + "', it is read-only.", fn)
		{

		}
	};

	class LuaBadWriteException : public LuaException
	{
	public:
		LuaBadWriteException(const string& prop, const string& reason, const string& fn)
			: LuaException("Cannot write to property '" + prop + "', " + reason, fn)
		{

		}
	};

	class LuaBadReadException : public LuaException
	{
	public:
		LuaBadReadException(const string& prop, const string& reason, const string& fn)
			: LuaException("Cannot read property '" + prop + "', " + reason, fn)
		{

		}
	};

	/* All Lua metatable events (http://lua-users.org/wiki/MetatableEvents). Description
		are copy paster from the wiki. */
	enum ScriptOperator
	{
		/* Control 'prototype' inheritance. When accessing "myTable[key]" and the key does not appear in the table, but the metatable has an __index property:

			if the value is a function, the function is called, passing in the table and the key; the return value of that function is returned as the result.
			if the value is another table, the value of the key in that table is asked for and returned
				(and if it doesn't exist in that table, but that table's metatable has an __index property, then it continues on up) 
			Use "rawget(myTable,key)" to skip this metamethod.  */
		script_operator_index, 
		/* Control property assignment. When calling "myTable[key] = value", if the metatable has a __newindex key pointing to a function, call that function, passing it the table, key, and value.

			Use "rawset(myTable,key,value)" to skip this metamethod.
			(If the __newindex function does not set the key on the table (using rawset) then the key/value pair is not added to myTable.)  */
		script_operator_newindex,
		/* Control weak references. A string value with one or both of the characters 'k' and 'v' which specifies that the the keys and/or values in the table are weak references.  */
		script_operator_mode,
		/* Treat a table like a function. When a table is followed by parenthesis such as "myTable( 'foo' )" and the metatable has a __call key pointing to a function, that function is invoked (passing the table as the first argument, followed by any specified arguments) and the return value is returned.  */
		script_operator_call,
		/* Hide the metatable. When "getmetatable( myTable )" is called, if the metatable for myTable has a __metatable key, the value of that key is returned instead of the actual metatable.  */
		script_operator_metatable,
		/*  Control string representation. When the builtin "tostring( myTable )" function is called, if the metatable for myTable has a __tostring property set to a function, that function is invoked (passing myTable to it) and the return value is used as the string representation.  */
		script_operator_tostring,
		/* Control table length that is reported. When the table length is requested using the length operator ( '#' ), if the metatable for myTable has a __len key pointing to a function, that function is invoked (passing myTable to it) and the return value used as the value of "#myTable".  */
		script_operator_len,
		/* (Lua 5.2+) Handle iteration through table pairs when for k,v in pairs(tbl) do ... end is called */
		script_operator_pairs,
		/* Handle iteration through table pairs when for k,v in ipairs(tbl) do ... end is called */
		script_operator_ipairs,
		/* Userdata finalizer code. When userdata is set to be garbage collected, if the metatable has a __gc field pointing to a function, that function is first invoked, passing the userdata to it. The __gc metamethod is not called for tables. (See http://lua-users.org/lists/lua-l/2006-11/msg00508.html)  */
		script_operator_gc,
		
		/* Unary minus. When writing "-myTable", if the metatable has a __unm key pointing to a function, that function is invoked (passing the table), and the return value used as the value of "-myTable".  */
		script_operator_unm,
		/* Addition. When writing "myTable + object" or "object + myTable", if myTable's metatable has an __add key pointing to a function, that function is invoked (passing the left and right operands in order) and the return value used.  
		'If both operands are tables, the left table is checked before the right table for the presence of an __add metaevent. */
		script_operator_add,
		/* Subtraction. Invoked similar to addition, using the '-' operator.  */
		script_operator_sub,
		/* Multiplication. Invoked similar to addition, using the '*' operator.  */
		script_operator_mul,
		/* Division. Invoked similar to addition, using the '/' operator.  */
		script_operator_div,
		/* Floor division (division with rounding down to nearest integer). '//' operator.  */
		script_operator_idiv,
		/* Modulo. Invoked similar to addition, using the '%' operator.  */
		script_operator_mod,
		/* Involution. Invoked similar to addition, using the '^' operator.  */
		script_operator_pow,
		/* Concatenation. Invoked similar to addition, using the '..' operator.  */
		script_operator_concat,

		/* the bitwise AND (&) operation.  */
		script_operator_band,
		/* the bitwise OR (|) operation.  */
		script_operator_bor,
		/* the bitwise exclusive OR (binary ~) operation.  */
		script_operator_bxor,
		/* the bitwise NOT (unary ~) operation.  */
		script_operator_bnot,
		/* the bitwise left shift (<<) operation. */
		script_operator_shl,
		/* the bitwise right shift (>>) operation. */
		script_operator_shr,

		/* Check for equality. This method is invoked when "myTable1 == myTable2" is evaluated, but only if both tables have the exact same metamethod for __eq.  */
		script_operator_eq,
		/* Check for less-than. Similar to equality, using the '<' operator.  */
		script_operator_lt,
		/* Check for less-than-or-equal. Similar to equality, using the '<=' operator.  */
		script_operator_le,
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
		bool InvokeLuaFunction(int32 index, TArg... args, int32 nret = 0);
		bool InvokeLuaFunctionStackArgs(int32 index, std::vector<int32> stackArgs = std::vector<int32>(), int32 nret = 0);
		int32 GetInteger(int32 index) const;
		float GetNumber(int32 index) const;
		bool GetBoolean(int32 index) const;
		string GetString(int32 index) const;
		PropertyTable GetTable(int32 index);
		template <typename T>
		T Get(int32 index);

		template <typename ...TArg>
		bool InvokeLuaFunction(const string& name, TArg... args, int32 nret = 0);
		bool InvokeLuaFunctionStackArgs(const string& name, std::vector<int32> stackArgs = std::vector<int32>(), int32 nret = 0);
		int32 GetInteger(const string& key) const;
		float GetNumber(const string& key) const;
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

		void PushNil();

		void PushCopy(int32 index);
		void PushCopy(const string& key);

		void Pop(int32 index);

		void SetFunction(const string& key, lua_CFunction value);
		void SetInteger(const string& key, int32 value);
		void SetNumber(const string& key, float value);
		void SetBoolean(const string& key, bool value);
		void SetString(const string& key, const char* value);
		void SetTable(const string& key, const PropertyTable& table);

		void SetFieldInteger(int32 tableIndex, const string& fieldName, int32 value);
		void SetFieldNumber(int32 tableIndex, const string& fieldName, float value);
		void SetFieldBoolean(int32 tableIndex, const string& fieldName, bool value);
		void SetFieldString(int32 tableIndex, const string& fieldName, const char* value);
		void SetFieldSubtable(int32 target, const string& fieldName, const PropertyTable& wzTable);
		void SetFieldSubtable(int32 target, const string& fieldName, const string& luaTable);
		void SetFieldSubtable(int32 target, const string& fieldName, int32 luaTable);
		void SetTableFunction(int32 tableIndex, const string& fnName, lua_CFunction fn);
		bool InvokeTableFunction(int32 tableIndex, const string& fnName);
		void MakeTableReference(int32 tableIndex, const string& ref);

		void SetFieldInteger(const string& key, const string& fieldName, int32 value);
		void SetFieldNumber(const string& key, const string& fieldName, float value);
		void SetFieldBoolean(const string& key, const string& fieldName, bool value);
		void SetFieldString(const string& key, const string& fieldName, const char* value);
		void SetFieldSubtable(const string& target, const string& fieldName, const PropertyTable& wzTable);
		void SetFieldSubtable(const string& target, const string& fieldName, const string& luaTable);
		void SetFieldSubtable(const string& target, const string& fieldName, int32 luaTable);
		void SetTableFunction(const string& tableKey, const string& fnName, lua_CFunction fn);
		bool InvokeTableFunction(const string& tableKey, const string& fnName);
		void MakeTableReference(const string& src, const string& ref);


		bool IsFieldInteger(int32 table, const string& field);
		bool IsFieldNumber(int32 table, const string& field);
		bool IsFieldBoolean(int32 table, const string& field);
		bool IsFieldString(int32 table, const string& field);
		bool IsFieldTable(int32 table, const string& field);
		bool IsFieldFunction(int32 table, const string& field);

		bool IsFieldInteger(const string& table, const string& field);
		bool IsFieldNumber(const string& table, const string& field);
		bool IsFieldBoolean(const string& table, const string& field);
		bool IsFieldString(const string& table, const string& field);
		bool IsFieldTable(const string& table, const string& field);
		bool IsFieldFunction(const string& table, const string& field);

		void CreateMetaTable(const string& metaTable);
		void SetMetaMethod(const string& metaTable, ScriptOperator op, lua_CFunction fn);
		void SetMetaTable(int32 tableIndex, const string& metaTable);
		void SetMetaTable(const string& table, const string& metaTable);

		void SetNil(const string& key);

		string GetTypeName(int32 index);
		string GetTypeName(const string& key);

		inline lua_State* GetLuaState() { return L; };

		int32 GetStackCount();

		size_t GetMemoryUsage();

	private:
		string TypeStrToScriptTypeName(const string& typeStr);
		PropertyTable ToWizzyTable(int32 luaStackIndex);
		string OperatorString(ScriptOperator op);
		bool InvokeFromStack(int32 narg, int32 nret);

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
	inline bool ScriptContext::InvokeLuaFunction(int32 index, TArg ...args, int32 nret)
	{
		WZ_CORE_ASSERT(lua_isfunction(L, index), "Invalid context function call");

		constexpr std::size_t argc = sizeof...(TArg);

		PushCopy(index);

		((void)Push(args), ...);

		bool result = InvokeFromStack(argc, nret);

		Pop(1);

		return result;
	}
	template<typename ...TArg>
	inline bool ScriptContext::InvokeLuaFunction(const string& name, TArg ...args, int32 nret)
	{
		lua_getglobal(L, name.c_str());
		int32 idx = lua_gettop(L);
		bool success = InvokeLuaFunction(idx, args..., nret);
		return success;
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