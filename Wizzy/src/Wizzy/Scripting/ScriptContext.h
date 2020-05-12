#pragma once

#include "Wizzy/Scripting/Script.h"
#include "Wizzy/Scripting/ScriptContextAPI.h"
#include "Wizzy/WizzyExceptions.h"


namespace Wizzy
{
	

	class ScriptContext
	{
	public:
		ScriptContext(ScriptLang lang);
		~ScriptContext();

		template <typename T>
        inline void Set(const string& key, const T& value)
		{
			m_api->Set(key, value);
		}

        template <typename T>
        inline void Get(const string& key)
		{
			return m_api->Get<T>(key);
		}

        template <typename T>
        void Is(const string& key)
		{
			return m_api->Is<T>(key);
		}

	private:
		ScriptContextAPI* m_api;
	};
}