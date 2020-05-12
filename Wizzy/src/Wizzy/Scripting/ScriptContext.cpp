#include "wzpch.h"

#include <squirrel.h>
#include <lua.hpp>

#include "Wizzy/Scripting/ScriptContext.h"
#include "Wizzy/Resource/ResourceManagement.h"

namespace Wizzy
{
	ScriptContext::ScriptContext(ScriptLang lang)
	{
		m_api = ScriptContextAPI::Create(lang);
	}
	ScriptContext::~ScriptContext()
	{
		delete m_api;
	}
}