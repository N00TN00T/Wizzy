
#include "Wizzy/Scripting/Script.h"
#include "Wizzy/PropertyTable.h"
#include "Wizzy/WizzyExceptions.h"

namespace Wizzy
{
    class ScriptRuntimeException : public Exception
    {
    public:
        ScriptRuntimeException(const string& msg, const string& fn) : Exception(msg, 0, fn)
        {

        }

        inline string GetMessage() const override 
        { 
            return "Script Runtime Error:\nIn Function: " + m_function + "\nMessage:" + m_message; 
        }
		inline string GetFunction() const override { return m_function; }

		inline virtual string GetUnhandledMessage() const override
		{
			return string("Unhandled Script Runtime Exception!\n    ")
					+ GetMessage();
		}
    };

    enum class ScriptDataType : u8
    {
        _int, _float, _string, _bool, _object, _objectRef, _function, _null
    };

    typedef s64 	        ScriptInt;
	typedef f64 	        ScriptFloat;
	typedef string 	        ScriptString;
	typedef bool 	        ScriptBool;
    typedef PropertyTable   ScriptObject;
    typedef ScriptObject*   ScriptObjectRef;
    

    struct ScriptBinaryFunction
    {
        byte* byteCode;
    };
    
    template <typename T>
    struct ScriptFunctionDecl
    {
        ScriptFunctionDecl(T fn) : fn(fn)
        typedef T Type;
        T fn;

        operator T()
        {
            return fn;
        }
    };

    template <ScriptLang LANG>
    class ScriptContextAPI
    {
    public:
        struct ScriptCall
        {
            friend class ScriptContextAPI;

            u16 numArgs = 0;

            template <typename T>
            T GetArg(u16 argIdx)
            {
                return context->GetIndexed<T>(argIndex);
            }

            template <typename T>
            inline bool IsArg(u16 argIdx)
            {
                return context->IsIndex<T>(argIdx);
            }

            template <typename T>
            inline void Return(const T& v)
            {
                if (context->SupportsMultiReturn()) 
                {
                    WZ_CORE_ASSERT(!hasReturned, 
                        "Only 1 return value allowed in " 
                        + ScriptLangToString(context->GetContextLang()));
                }
                context->Push(v);
                hasReturned = true;
            }

            ScriptDataType GetArgType(u16 argIdx);

            inline bool HasSignature(std::vector<ScriptDataType> sig)
            {
                if (this->numArgs < sig.size()) return false;

                for (s16 i = 0; i < sig.size(); i++)
                {
                    if (this->GetArgType(i) != sig[i]) return false;
                }

                return true;
            }

        private:
            ScriptContextAPI* context;
            bool hasReturned = false;
        };

    public:
        virtual ~ScriptContextAPI() {}

        template <typename T>
        inline void ScriptContextAPI::Set(const string& key, const T& value)
        {
            if constexpr(std::is_same<T, ScriptBool>())
            {
                this->SetBool(key, value);
            }
            else if (std::is_integral<T>())
            {
                this->SetInt(key, value);
            }
            else if (std::is_floating_point<T>())
            {
                this->SetFloat(key, value);
            }
            else if (IS_STRING(T))
            {
                this->SetString(key, value);
            }
            else if (std::is_same<T, ScriptObject>())
            {
                this->SetObject(key, value);
            }
            else if (std::is_same<T, ScriptObjectRef>())
            {
                this->SetObjectRef(key, value);
            }
            else if (std::is_same<T, ScriptCFunction>)
            {
                this->SetFunction(key, value);
            }
            else 
            {
                static_assert(false, "Invalid type in ScriptContextAPI::Set");
            }
        }

        template <typename T>
        inline T Get(const string& key)
        {
            if constexpr(std::is_same<T, ScriptBool>())
            {
                return this->GetBool(key);
            }
            else if (std::is_integral<T>())
            {
                return this->GetInt(key);
            }
            else if (std::is_floating_point<T>())
            {
                return this->GetFloat(key);
            }
            else if (IS_STRING(T))
            {
                return this->GetString(key);
            }
            else if (std::is_same<T, ScriptObject>())
            {
                return this->GetObject(key);
            }
            else if (std::is_same<T, ScriptObjectRef>())
            {
                return this->GetObjectRef(key);
            }
            else if (std::is_same<T, ScriptCFunction>)
            {
                return this->GetFunction(key);
            }
            else 
            {
                static_assert(false, "Invalid type in ScriptContextAPI::Get");
            }
        }

        template <typename T>
        inline bool Is(const string& key)
        {
            if constexpr(std::is_same<T, ScriptBool>())
            {
                return this->IsBool(key);
            }
            else if (std::is_integral<T>())
            {
                return this->IsInt(key);
            }
            else if (std::is_floating_point<T>())
            {
                return this->IsFloat(key);
            }
            else if (IS_STRING(T))
            {
                return this->IsString(key);
            }
            else if (std::is_same<T, ScriptObject>())
            {
                return this->IsObject(key);
            }
            else if (std::is_same<T, ScriptObjectRef>())
            {
                return this->IsObjectRef(key);
            }
            else if (std::is_same<T, ScriptCFunction>)
            {
                return this->IsFunction(key);
            }
            else 
            {
                static_assert(false, "Invalid type in ScriptContextAPI::Is");
            }
        }

        template <typename TRet, typename ...TArg>
        inline std::shared_ptr<TRet> InvokeFunction(const string& key, TArg ...args)
        {
            this->PushGlobal(key);
            WZ_CORE_ASSERT(IsIndexFunction(this->GetTop()), 
                "'" + key + "' is not a global function");

            constexpr u16 argc = (u16)sizeof...(TArg);

            ((void)Push(args), ...);

            InvokeFunctionIndexed(-1 - argc, key);

            this->Pop(1);

            return result;
        }
        
        virtual bool SupportsMultiReturn() const = 0;
        virtual ScriptLang GetContextLang() const = 0;

        virtual ScriptDataType GetType(const string& key) const = 0;

        virtual void SetInt         (const string& key, const ScriptInt& v) = 0;
        virtual void SetFloat       (const string& key, const ScriptFloat& v) = 0;
        virtual void SetString      (const string& key, const ScriptString& v) = 0;
        virtual void SetBool        (const string& key, ScriptBool v) = 0;
        virtual void SetObject      (const string& key, const ScriptObject& v) = 0;
        virtual void SetObjectRef   (const string& key, ScriptObjectRef v) = 0;
        virtual void SetFunction    (const string& key, ScriptCFunction v) = 0;
        virtual void SetNull        (const string& key) = 0;

        virtual ScriptInt       GetInt          (const string& key) const = 0;
        virtual ScriptFloat     GetFloat        (const string& key) const = 0;
        virtual ScriptString    GetString       (const string& key) const = 0;
        virtual ScriptBool      GetBool         (const string& key) const = 0;
        virtual ScriptObject    GetObject       (const string& key) const = 0;
        virtual ScriptObjectRef GetObjectRef    (const string& key) const = 0;
        virtual ScriptBinaryFunction  GetFunction     (const string& key) const = 0;

        virtual bool IsInt          (const string& key) const = 0;
        virtual bool IsFloat        (const string& key) const = 0;
        virtual bool IsString       (const string& key) const = 0;
        virtual bool IsBool         (const string& key) const = 0;
        virtual bool IsObject       (const string& key) const = 0;
        virtual bool IsObjectRef    (const string& key) const = 0;
        virtual bool IsFunction     (const string& key) const = 0;
        virtual bool IsNull         (const string& key) const = 0;

    protected:
        template <typename T>
        inline void Push(const T& value)
        {
            if constexpr(std::is_same<T, ScriptBool>())
            {
                this->PushBool(value);
            }
            else if (std::is_integral<T>())
            {
                this->PushInt(value);
            }
            else if (std::is_floating_point<T>())
            {
                this->PushFloat(value);
            }
            else if (IS_STRING(T))
            {
                this->PushString(value);
            }
            else if (std::is_same<T, ScriptObject>())
            {
                this->PushObject(value);
            }
            else if (std::is_same<T, ScriptObjectRef>())
            {
                this->PushObjectRef(value);
            }
            else if (std::is_same<T, ScriptCFunction>)
            {
                this->PushFunction(value);
            }
            else 
            {
                static_assert(false, "Invalid type in ScriptContextAPI::Push");
            }
        }

        template <typename T>
        inline T GetIndexed(s16 idx)
        {
            if constexpr(std::is_same<T, ScriptBool>())
            {
                return this->GetBoolIndexed(idx);
            }
            else if (std::is_integral<T>())
            {
                return this->GetIntIndexed(idx);
            }
            else if (std::is_floating_point<T>())
            {
                return this->GetFloatIndexed(idx);
            }
            else if (IS_STRING(T))
            {
                return this->GetStringIndexed(idx);
            }
            else if (std::is_same<T, ScriptObject>())
            {
                return this->GetObjectIndexed(idx);
            }
            else if (std::is_same<T, ScriptObjectRef>())
            {
                return this->GetObjectRefIndexed(idx);
            }
            else if (std::is_same<T, ScriptCFunction>)
            {
                return this->GetFunctionIndexed(idx);
            }
            else 
            {
                static_assert(false, "Invalid type in ScriptContextAPI::GetIndexed");
            }
        }

        template <typename T>
        inline bool IsIndex(s16 idx)
        {
            if constexpr(std::is_same<T, ScriptBool>())
            {
                return this->IsIndexBool(idx);
            }
            else if (std::is_integral<T>())
            {
                return this->IsIndexInt(idx);
            }
            else if (std::is_floating_point<T>())
            {
                return this->IsIndexFloat(idx);
            }
            else if (IS_STRING(T))
            {
                return this->IsIndexString(idx);
            }
            else if (std::is_same<T, ScriptObject>())
            {
                return this->IsIndexObject(idx);
            }
            else if (std::is_same<T, ScriptObjectRef>())
            {
                return this->IsIndexObjectRef(idx);
            }
            else if (std::is_same<T, ScriptCFunction>)
            {
                return this->IsIndexFunction(idx);
            }
            else 
            {
                static_assert(false, "Invalid type in ScriptContextAPI::IsIndex");
            }
        }

        virtual void InvokeFunctionFromStack(u16 narg, const string& fnId) = 0;

        virtual void Pop(s16 c) const = 0;

        virtual ScriptDataType GetTypeIndexed(s32 idx) const = 0;

        virtual void PushGlobal(const string& key) const = 0;
        virtual void PushIndex(s16 idx) const = 0;
        // Push null
        inline void Push() const { PushNull(); }
        virtual s16 GetTop() const = 0;

        virtual void PushInt          (const ScriptInt& v) const = 0;
        virtual void PushFloat        (const ScriptFloat& v) const = 0;
        virtual void PushString       (const ScriptString& v) const = 0;
        virtual void PushBool         (ScriptBool v) const = 0;
        virtual void PushObject       (const ScriptObject& v) const = 0;
        virtual void PushObjectRef    (ScriptObjectRef v) const = 0;
        virtual void PushFunction     (ScriptCFunction v) const = 0;
        virtual void PushNull         () const = 0;

        virtual ScriptInt       GetIntIndexed          (s16 idx) const = 0;
        virtual ScriptFloat     GetFloatIndexed        (s16 idx) const = 0;
        virtual ScriptString    GetStringIndexed       (s16 idx) const = 0;
        virtual ScriptBool      GetBoolIndexed         (s16 idx) const = 0;
        virtual ScriptObject    GetObjectIndexed       (s16 idx) const = 0;
        virtual ScriptObjectRef GetObjectRefIndexed    (s16 idx) const = 0;
        virtual ScriptFunction  GetFunctionIndexed     (s16 idx) const = 0;

        virtual bool IsIndexInt         (s16 idx) const = 0;
        virtual bool IsIndexFloat       (s16 idx) const = 0;
        virtual bool IsIndexString      (s16 idx) const = 0;
        virtual bool IsIndexBool        (s16 idx) const = 0;
        virtual bool IsIndexObject      (s16 idx) const = 0;
        virtual bool IsIndexObjectRef   (s16 idx) const = 0;
        virtual bool IsIndexFunction    (s16 idx) const = 0;
        virtual bool IsIndexNull        (s16 idx) const = 0;
    };
}