#include "wzpch.h"

#include "Wizzy/Scripting/ScriptManager.h"

#define READ_ONLY_PROP(name, ret, fn, type)\
.addProperty<ret, float>(name, static_cast<std::function<ret(const type*)>>([](const type* v) fn), [](type* t, float s) { throw LuaIllegalWriteException(name); })

#define READ_ONLY_STATIC_PROP(name, ret, fn)\
.addStaticProperty<ret>(name, []() fn, [](ret s) { throw LuaIllegalWriteException(name); } )

namespace Wizzy {

	int vec2_ctor(lua_State* L) {
		int _argc = lua_gettop(L);

		switch (_argc) {
			case 1:
			{
				luabridge::push(L, vec2(0, 0));
				break;
			}
			case 2:
			{
				if (!lua_isnumber(L, -1)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -1)));
				}

				luabridge::push(L, vec2(lua_tonumber(L, -1)));
				break;
			}
			case 3:
			{
				if (!lua_isnumber(L, -1)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -1)));
				}
				if (!lua_isnumber(L, -2)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -2)));
				}

				luabridge::push(L, vec2(lua_tonumber(L, -2), lua_tonumber(L, -1)));
				break;
			}
			default: 
			{
				throw LuaBadArgCException("[0, 1, 2]", std::to_string(_argc - 1));
			}
		}

		return 1;
	}
	int vec3_ctor(lua_State* L) {
		int _argc = lua_gettop(L);

		switch (_argc) {
			case 1:
			{
				luabridge::push(L, vec3(0));
				break;
			}
			case 2:
			{
				if (!lua_isnumber(L, -1)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -1)));
				}

				luabridge::push(L, vec3(lua_tonumber(L, -1)));
				break;
			}
			case 4:
			{
				if (!lua_isnumber(L, -1)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -1)));
				}
				if (!lua_isnumber(L, -2)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -2)));
				}
				if (!lua_isnumber(L, -3)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -3)));
				}

				luabridge::push(L, vec3(lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1)));
				break;
			}
			default:
			{
				throw LuaBadArgCException("[0, 1, 3]", std::to_string(_argc - 1));
			}
		}

		return 1;
	}
	int vec4_ctor(lua_State* L) {
		int _argc = lua_gettop(L);

		switch (_argc) {
			case 1:
			{
				luabridge::push(L, vec4(0));
				break;
			}
			case 2:
			{
				if (!lua_isnumber(L, -1)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -1)));
				}

				luabridge::push(L, vec4(lua_tonumber(L, -1)));
				break;
			}
			case 5:
			{
				if (!lua_isnumber(L, -1)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -1)));
				}
				if (!lua_isnumber(L, -2)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -2)));
				}
				if (!lua_isnumber(L, -3)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -3)));
				}
				if (!lua_isnumber(L, -4)) {
					throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -4)));
				}

				luabridge::push(L, vec4(lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1)));
				break;
			}
			default:
			{
				throw LuaBadArgCException("[0, 1, 4]", std::to_string(_argc - 1));
			}
		}

		return 1;
	}
	int color_ctor(lua_State* L) {
		int _argc = lua_gettop(L);

		switch (_argc) {
		case 1:
		{
			luabridge::push(L, Color(0, 0, 0, 0));
			break;
		}
		case 2:
		{
			if (!lua_isnumber(L, -1)) {
				throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -1)));
			}
			
			auto _num = lua_tonumber(L, -1);
			luabridge::push(L, Color(_num, _num, _num, _num));
			break;
		}
		case 4:
		{
			if (!lua_isnumber(L, -1)) {
				throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -1)));
			}
			if (!lua_isnumber(L, -2)) {
				throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -2)));
			}
			if (!lua_isnumber(L, -3)) {
				throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -3)));
			}

			luabridge::push(L, Color(lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1), 1));
			break;
		}
		case 5:
		{
			if (!lua_isnumber(L, -1)) {
				throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -1)));
			}
			if (!lua_isnumber(L, -2)) {
				throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -2)));
			}
			if (!lua_isnumber(L, -3)) {
				throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -3)));
			}
			if (!lua_isnumber(L, -4)) {
				throw LuaBadArgTException("number", lua_typename(L, lua_type(L, -4)));
			}

			luabridge::push(L, Color(lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1)));
			break;
		}
		default:
		{
			throw LuaBadArgCException("[0, 1, 3, 4]", std::to_string(_argc - 1));
		}
		}

		return 1;
	}

	std::unordered_map<ScriptEnum, u32> ScriptManager::s_stateIndex;
	std::vector<lua_State*> ScriptManager::s_states;


	lua_State* ScriptManager::GetState(ScriptEnum scriptType) {
		bool _isNew = s_stateIndex.emplace(scriptType, 0).second;

		if (_isNew) {
			s_stateIndex[scriptType] = s_states.size();
			s_states.push_back(CreateState(scriptType));
		}

		return s_states[s_stateIndex[scriptType]];
	}

	void ScriptManager::Shutdown() {
		for (auto _state : s_states) if (_state) lua_close(_state);
	}

	void ExposeMathLibraries(lua_State *L) {
		luabridge::getGlobalNamespace(L)

			.beginClass<vec2>("Vector2")
				.addStaticFunction("new", vec2_ctor)
				.addProperty<float, float>("x", static_cast<std::function<float(const vec2*)>>(
					[](const vec2* v) { return v->x; }),
					[](vec2* t, float v) { t->x = v; })
				//.addStaticProperty<vec2>("Zero", ([]() { return vec2(0); }), [](vec2 s) { throw LuaIllegalWriteException("Zero"); })
				.addProperty<float, float>("y", static_cast<std::function<float(const vec2*)>>(
					[](const vec2* v) { return v->y; }),
					[](vec2* t, float v) { t->y = v; })
				.addFunction<vec2, vec2>("__add", [](vec2* lhs, vec2 rhs) { return (*lhs) + rhs; })
				.addFunction<vec2, vec2>("__sub", [](vec2* lhs, vec2 rhs) { return (*lhs) - rhs; })
				.addFunction<vec2, vec2>("__mul", [](vec2* lhs, vec2 rhs) { return (*lhs) * rhs; })
				.addFunction<vec2, vec2>("__div", [](vec2* lhs, vec2 rhs) { return (*lhs) / rhs; })
				READ_ONLY_PROP("___T_", string, { return "Vector2"; }, vec2)
				READ_ONLY_PROP("Invert", vec2, { return -(*v); }, vec2)
				READ_ONLY_PROP("Magnitude", float, { return std::sqrt(v->x * v->x + v->y * v->y); }, vec2)
				READ_ONLY_PROP("Normalized", vec2, { float m = std::sqrt(v->x * v->x + v->y * v->y); return m == 0 ? vec2(0) : (*v / m); }, vec2)
				READ_ONLY_STATIC_PROP("Zero", vec2, { return vec2(0); })

			.endClass()


			.beginClass<vec3>("Vector3")
				.addStaticFunction("new", vec3_ctor)
				.addProperty<float, float>("x", static_cast<std::function<float(const vec3*)>>(
					[](const vec3* v) { return v->x; }),
					[](vec3* t, float v) { t->x = v; })
				.addProperty<float, float>("y", static_cast<std::function<float(const vec3*)>>(
					[](const vec3* v) { return v->y; }),
					[](vec3* t, float v) { t->y = v; })
				.addProperty<float, float>("z", static_cast<std::function<float(const vec3*)>>(
					[](const vec3* v) { return v->z; }),
					[](vec3* t, float v) { t->z = v; })
				.addFunction<vec3, vec3>("__add", [](vec3* lhs, vec3 rhs) { return (*lhs) + rhs; })
				.addFunction<vec3, vec3>("__sub", [](vec3* lhs, vec3 rhs) { return (*lhs) - rhs; })
				.addFunction<vec3, vec3>("__mul", [](vec3* lhs, vec3 rhs) { return (*lhs) * rhs; })
				.addFunction<vec3, vec3>("__div", [](vec3* lhs, vec3 rhs) { return (*lhs) / rhs; })
				READ_ONLY_PROP("ToVector2", vec2, { return vec2(v->x, v->y); }, vec3)
				READ_ONLY_PROP("___T_", string, { return "Vector3"; }, vec3)
				READ_ONLY_PROP("Invert", vec3, { return -(*v); }, vec3)
				READ_ONLY_PROP("Magnitude", float, { return std::sqrt(v->x * v->x + v->y * v->y + v->z * v->z); }, vec3)
				READ_ONLY_PROP("Normalized", vec3, { double m = std::sqrt(v->x * v->x + v->y * v->y + v->z * v->z);  return m != 0 ? vec3(0) : *v / (float)m; }, vec3)
				READ_ONLY_STATIC_PROP("Zero", vec3, { return vec3(0); })
			.endClass()

			.beginClass<vec4>("Vector4")
				.addStaticFunction("new", vec4_ctor)
				.addProperty<float, float>("x", static_cast<std::function<float(const vec4*)>>(
					[](const vec4* v) { return v->x; }),
					[](vec4* t, float v) { t->x = v; })
				.addProperty<float, float>("y", static_cast<std::function<float(const vec4*)>>(
					[](const vec4* v) { return v->y; }),
					[](vec4* t, float v) { t->y = v; })
				.addProperty<float, float>("z", static_cast<std::function<float(const vec4*)>>(
					[](const vec4* v) { return v->z; }),
					[](vec4* t, float v) { t->z = v; })
				.addProperty<float, float>("w", static_cast<std::function<float(const vec4*)>>(
					[](const vec4* v) { return v->w; }),
					[](vec4* t, float v) { t->w = v; })
				.addFunction<vec4, vec4>("__add", [](vec4* lhs, vec4 rhs) { return (*lhs) + rhs; })
				.addFunction<vec4, vec4>("__sub", [](vec4* lhs, vec4 rhs) { return (*lhs) - rhs; })
				.addFunction<vec4, vec4>("__mul", [](vec4* lhs, vec4 rhs) { return (*lhs) * rhs; })
				.addFunction<vec4, vec4>("__div", [](vec4* lhs, vec4 rhs) { return (*lhs) / rhs; })
				READ_ONLY_PROP("ToVector2", vec2, { return vec2(v->x, v->y); }, vec4)
				READ_ONLY_PROP("ToVector3", vec3, { return vec3(v->x, v->y, v->z); }, vec4)
				READ_ONLY_PROP("___T_", string, { return "Vector4"; }, vec4)
				READ_ONLY_PROP("Invert", vec4, { return -(*v); }, vec4)
				READ_ONLY_PROP("Magnitude", float, { return std::sqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w); }, vec4)
				READ_ONLY_PROP("Normalized", vec4, { float m = std::sqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);  return m != 0 ? vec4(0) : *v / (float)m; }, vec4)
				READ_ONLY_STATIC_PROP("Zero", vec4, { return vec4(0); })
			.endClass()

			.beginClass<Color>("Color")
				.addStaticFunction("new", color_ctor)
				.addProperty<float, float>("r", static_cast<std::function<float(const Color*)>>(
					[](const Color* v) { return v->r; }),
					[](Color* t, float v) { t->r = v; })
				.addProperty<float, float>("g", static_cast<std::function<float(const Color*)>>(
					[](const Color* v) { return v->g; }),
					[](Color* t, float v) { t->g = v; })
				.addProperty<float, float>("b", static_cast<std::function<float(const Color*)>>(
					[](const Color* v) { return v->b; }),
					[](Color* t, float v) { t->b = v; })
				.addProperty<float, float>("a", static_cast<std::function<float(const Color*)>>(
					[](const Color* v) { return v->a; }),
					[](Color* t, float v) { t->a = v; })
				.addFunction<Color, Color>("__add", [](Color* lhs, Color rhs) { return (*lhs) + rhs; })
				.addFunction<Color, Color>("__sub", [](Color* lhs, Color rhs) { return (*lhs) - rhs; })
				.addFunction<Color, Color>("__mul", [](Color* lhs, Color rhs) { return (*lhs) * rhs; })
				.addFunction<Color, Color>("__div", [](Color* lhs, Color rhs) { return (*lhs) / rhs; })
				READ_ONLY_PROP("ToVector4", vec4, { return vec4(v->r, v->g, v->b, v->a); }, Color)
				READ_ONLY_PROP("___T_", string, { return "Color"; }, Color)
				READ_ONLY_PROP("Invert", Color, { return Color(1.f - v->r, 1.f - v->g, 1.f - v->b, 1.f - v->a); }, Color)
				READ_ONLY_PROP("Magnitude", float, { return std::sqrt(v->r * v->r + v->g * v->g + v->b * v->b + v->a * v->a); }, Color)
				READ_ONLY_STATIC_PROP("Black", Color, { return Color::black; })
				READ_ONLY_STATIC_PROP("White", Color, { return Color::white; })
				READ_ONLY_STATIC_PROP("Red", Color, { return Color::red; })
				READ_ONLY_STATIC_PROP("Green", Color, { return Color::green; })
				READ_ONLY_STATIC_PROP("Blue", Color, { return Color::blue; })
				READ_ONLY_STATIC_PROP("Magenta", Color, { return Color::magenta; })
				READ_ONLY_STATIC_PROP("Violet", Color, { return Color::violet; })
				READ_ONLY_STATIC_PROP("Yellow", Color, { return Color::yellow; })
				READ_ONLY_STATIC_PROP("Gray", Color, { return Color::gray; })
			.endClass();
	}

	void ExposeConfigLibraires(lua_State* L) {
		lua_newtable(L);
		lua_setglobal(L, "Graphics");
		lua_remove(L, -1);
	}

	void ExposeLibraries(ScriptEnum scriptType, lua_State* L) {

		ExposeMathLibraries(L);

		// TODO: Expose scripttype-specific libraires

		switch (scriptType) {
		case SCRIPT_TYPE_CONFIG: ExposeConfigLibraires(L); break;
		case SCRIPT_TYPE_COMPONENT: /*ExposeComponentLibraires(L);*/ break;
		case SCRIPT_TYPE_SYSTEM: /*ExposeSystemLibraires(L);*/ break;
		default: WZ_CORE_ASSERT(false, "Unhandled or invalid script type");
		}
	}

	lua_State* ScriptManager::CreateState(ScriptEnum scriptType) {
		lua_State* _state = luaL_newstate();
		luaL_openlibs(_state);

		ExposeLibraries(scriptType, _state);

		return _state;
	}

}