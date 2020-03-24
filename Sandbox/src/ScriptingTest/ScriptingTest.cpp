#include "spch.h"
#include "ScriptingTest.h"

int lua_log_debug(lua_State* L)
{
	WZ_DEBUG("Lua message: '{0}'", lua_tostring(L, -1));
	return 0;
}

void ScriptingTest::Init()
{
	wz::ResourceManagement::SetResourceDir("../res/");

	wz::Script::Handle hScript = wz::ResourceManagement::Load<wz::Script>("test/scripts/test.lua");

	wz::ScriptContext ctx;

	ctx.SetFunction("log_debug", lua_log_debug);

	wz::PropertyTable someTable;
	someTable.SetProperty<int32>("value", 9);

	wz::PropertyTable subTable;
	subTable.SetProperty<int32>("value", 92);

	someTable.SetProperty<wz::PropertyTable>("subTable", subTable);

	ctx.SetTable("someTable", someTable);

	ctx.InvokeScript(hScript);

	ctx.InvokeLuaFunction("test");
}

void ScriptingTest::Shutdown()
{
}
