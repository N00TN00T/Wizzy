#include "wzpch.h"

#include "Wizzy/Scripting/ScriptContextAPI.h"
#include "Wizzy/Scripting/LuaScriptContextAPI.h"
#include "Wizzy/Scripting/SquirrelScriptContextAPI.h"
//#include "Wizzy/Scripting/WrenScriptContextAPI.h"

namespace Wizzy 
{

    // bool DoSomething(int a, float b, string c = "hi")
    /*void script_DoSomething(ScriptCall* call)
    {   
        bool correctArgs = call->HasSignature(
                            { 
                                ScriptDataType::_int,
                                ScriptDataType::_float
                            });
        if (!correctArgs)
        {
            // error, throw exception?
            return;
        }

        int a = call->GetArg<int>(0);
        float b = call->GetArg<float>(1);
        string c = call->IsArg<string>(2) ? call->GetArg<string>(2) : "hi";

        call->Return(a > 5 && b != 1.5f && c != "hi");
    }*/

    ScriptContextAPI* ScriptContextAPI::Create(ScriptLang lang)
    {
        switch (lang)
        {
            case script_lang_lua: return new LuaScriptContextAPI();
            case script_lang_squirrel: return new SquirrelScriptContextAPI();
            //case script_lang_wren: return new WrenScriptContextAPI();
            default: 
            {
                WZ_CORE_CRITICAL("Unimplemented script lang {0}", 
                        ScriptLangToString(lang)); 
                WZ_BREAK;
            }
        }

        return NULL;
    }
}