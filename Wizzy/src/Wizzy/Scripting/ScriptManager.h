#pragma once

#define EXC_RET_STR(str) \
char* _ret = new char[(str).size()]; \
\
strcpy(_ret, (str).c_str()); \
\
return _ret

namespace Wizzy {

	class LuaException : public std::exception {

	};

	class LuaBadArgCException : public LuaException {
	public:
		LuaBadArgCException(string goodCount, string actualCount)
			: good(goodCount), actual(actualCount) {

		}

		virtual char const* what() const override {
			EXC_RET_STR("Bad argument count (expected " +
				good + ", got " +
				actual + ").");
		}

	private:
		string good, actual;
	};

	class LuaBadArgTException : public LuaException {
	public:
		LuaBadArgTException(string goodType, string actualType)
			: good(goodType), actual(actualType) {

		}

		virtual char const* what() const override {
			EXC_RET_STR("Bad argument type (expected " + \
						good + ", got " + \
						actual + ").");
		}

	private:
		string good, actual;
	};

	class LuaIllegalWriteException : public LuaException {
	public:
		LuaIllegalWriteException(string prop)
			: prop(prop) {

		}

		virtual char const* what() const override {
			EXC_RET_STR("Cannot write to property '" + prop + "', it is read-only.");
		}

	private:
		string prop;
	};

	class ScriptManager {
	public:

		static
		lua_State* GetState(ScriptEnum scriptType);

		static
		void Shutdown();

	private:
		static
		lua_State* CreateState(ScriptEnum scriptType);

	private:
		static
		std::unordered_map<ScriptEnum, u32> s_stateIndex;
		static
		std::vector<lua_State*> s_states;
	};
}