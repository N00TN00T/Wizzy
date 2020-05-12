
#include "Wizzy/Scripting/ScriptContextAPI.h"

#include <lua.hpp>

namespace Wizzy 
{
    class LuaScriptContextAPI : public ScriptContextAPI
    {
    public:
        LuaScriptContextAPI();
        ~LuaScriptContextAPI();
        
        bool SupportsMultiReturn() const override;
        ScriptLang GetContextLang() const override;

        ScriptDataType GetType(const string& key) const override;

        void SetInt         (const string& key, const ScriptInt& v) override;
        void SetFloat       (const string& key, const ScriptFloat& v) override;
        void SetString      (const string& key, const ScriptString& v) override;
        void SetBool        (const string& key, ScriptBool v) override;
        void SetObject      (const string& key, const ScriptObject& v) override;
        void SetObjectRef   (const string& key, ScriptObjectRef v) override;
        void SetFunction    (const string& key, ScriptCFunction v) override;
        void SetNull        (const string& key) override;

        ScriptInt       GetInt          (const string& key) const override;
        ScriptFloat     GetFloat        (const string& key) const override;
        ScriptString    GetString       (const string& key) const override;
        ScriptBool      GetBool         (const string& key) const override;
        ScriptObject    GetObject       (const string& key) const override;
        ScriptObjectRef GetObjectRef    (const string& key) const override;
        ScriptFunction  GetFunction     (const string& key) const override;

        bool IsInt          (const string& key) const override;
        bool IsFloat        (const string& key) const override;
        bool IsString       (const string& key) const override;
        bool IsBool         (const string& key) const override;
        bool IsObject       (const string& key) const override;
        bool IsObjectRef    (const string& key) const override;
        bool IsFunction     (const string& key) const override;
        bool IsNull         (const string& key) const override;

    private:

        void InvokeFunctionFromStack(u16 narg, const string& fnId) override;

        void Pop(s16 c) const override;

        ScriptDataType GetTypeIndexed(s32 idx) const override;

        void PushGlobal(const string& key) const override;
        void PushIndex(s16 idx) const override;
        s16 GetTop() const override;

        void PushInt          (const ScriptInt& v) const override;
        void PushFloat        (const ScriptFloat& v) const override;
        void PushString       (const ScriptString& v) const override;
        void PushBool         (ScriptBool v) const override;
        void PushObject       (const ScriptObject& v) const override;
        void PushObjectRef    (ScriptObjectRef v) const override;
        void PushFunction     (ScriptCFunction v) const override;
        void PushNull         () const override;

        ScriptInt       GetIntIndexed          (s16 idx) const override;
        ScriptFloat     GetFloatIndexed        (s16 idx) const override;
        ScriptString    GetStringIndexed       (s16 idx) const override;
        ScriptBool      GetBoolIndexed         (s16 idx) const override;
        ScriptObject    GetObjectIndexed       (s16 idx) const override;
        ScriptObjectRef GetObjectRefIndexed    (s16 idx) const override;
        ScriptFunction  GetFunctionIndexed     (s16 idx) const override;

        bool IsIndexInt         (s16 idx) const override;
        bool IsIndexFloat       (s16 idx) const override;
        bool IsIndexString      (s16 idx) const override;
        bool IsIndexBool        (s16 idx) const override;
        bool IsIndexObject      (s16 idx) const override;
        bool IsIndexObjectRef   (s16 idx) const override;
        bool IsIndexFunction    (s16 idx) const override;
        bool IsIndexNull        (s16 idx) const override;

    private:
        lua_State* L;
    };
}