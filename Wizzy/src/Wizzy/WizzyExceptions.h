#pragma once

#define WZ_THROW(t, ...) throw ::Wizzy::t(__VA_ARGS__, __LINE__, __FUNCTION__)

#define __RESOURCE_EXCEPTION_DECL(tpname)\
class tpname : public ResourceException\
{\
public:\
	tpname(const string& path, u32 id, const string& name, u32 line, const string& fn);\
	tpname(const string& path, u32 line, const string& fn);\
\
	virtual string GetPath() const override;\
	virtual u32 GetId() const override;\
	virtual string GetName() const override;\
\
protected:\
	virtual string GetSubMessage() const override;\
\
private:\
	string m_path;\
	u32 m_id;\
	string m_name;\
}\

namespace Wizzy
{

	class Exception
	{
	public:
		Exception() {}
		Exception(const string& message, int32 line, const string& function);

		inline virtual string GetMessage() const { return m_message; }
		inline virtual const int32 GetLine() const { return m_line; }
		inline virtual string GetFunction() const { return m_function; }

		inline virtual string GetUnhandledMessage() const
		{
			return string("Unhandled exception!\n    ")
					+ "Message: '" + this->GetMessage() + "'\n        "
					+ "In function '" + this->GetFunction() + "'\n        "
					+ "On line " + std::to_string(this->GetLine());
		}

	protected:
		string m_message;
		int32 m_line;
		string m_function;
	};

	class ResourceException : public Exception
	{
	public:
		using Exception::Exception;
		virtual string GetPath() const { return "N/A"; };
		virtual u32 GetId() const { return 0; };
		virtual string GetName() const { return "N/A"; };
		virtual string GetMessage() const override;
	protected:
		virtual string GetSubMessage() const { return "N/A"; };
	protected:
		const string m_messagePrefix = "[RESOURCE ERROR]";
		bool m_handle = false;
	};

	__RESOURCE_EXCEPTION_DECL(ResourceInvalidPathException);
	__RESOURCE_EXCEPTION_DECL(ResourceFileAccessException);
	__RESOURCE_EXCEPTION_DECL(ResourceUnloadedException);
	__RESOURCE_EXCEPTION_DECL(ResourceLoadedException);
	__RESOURCE_EXCEPTION_DECL(ResourceIndexOutOfRangeException);
	__RESOURCE_EXCEPTION_DECL(ResourceSourceChangedException);
	__RESOURCE_EXCEPTION_DECL(ResourceParseErrorException);
	__RESOURCE_EXCEPTION_DECL(ResourcePathTakenException);
	__RESOURCE_EXCEPTION_DECL(ResourceFileMissingException);
	
	class ResourceInvalidHandleException : public ResourceException
	{
	public:
		ResourceInvalidHandleException(u32 id, u32 line, const string& fn);
		virtual string GetMessage() const override;
	private:
		u32 m_id;
	};

}