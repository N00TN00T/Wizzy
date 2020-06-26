#include "wzpch.h"

#include "Wizzy/Exceptions/WizzyExceptions.h"
#include "Wizzy/Resource/Resource.h"

namespace Wizzy
{

#define __RESOURCE_EXCEPTION_IMPL(tname, msg)\
	tname::tname(const string& path, u32 id, const string& name, u32 line, const string& fn)\
	: ResourceException("", line, fn), m_path(path), m_id(id), m_name(name)\
	{\
		m_handle = true;\
	}\
	tname::tname(const string& path, u32 line, const string& fn)\
		: ResourceException("", line, fn), m_path(path), m_id(WZ_NULL_RESOURCE_HANDLE.id), m_name("N/A")\
	{\
		m_handle = false;\
	}\
	string tname::GetSubMessage() const\
	{\
		return msg;\
	}\
	string tname::GetPath() const\
	{\
		return m_path;\
	}\
	u32 tname::GetId() const\
	{\
		return m_id;\
	}\
	string tname::GetName() const\
	{\
		return m_name;\
	}\


	Exception::Exception(const string& message, int32 line, const string& function)
		: m_message(message), m_line(line), m_function(function)
	{

	}


	string ResourceException::GetMessage() const
	{
		string base = m_messagePrefix + " '" + GetSubMessage() + "'";
		if (m_handle)
		{
			return base + " Resource: { id: " + std::to_string(GetId()) + ", name: " + GetName() + " } Path: '" + GetPath() + "'";
		}
		else
		{
			return base + " Path: '" + GetPath() + "'";
		}
	}

	__RESOURCE_EXCEPTION_IMPL(ResourceFileAccessException,	"Could not access file");
	__RESOURCE_EXCEPTION_IMPL(ResourceInvalidPathException, "Invalid path");
	__RESOURCE_EXCEPTION_IMPL(ResourceUnloadedException,	"Resource unloaded");
	__RESOURCE_EXCEPTION_IMPL(ResourceLoadedException,		"Resource is loaded but it should not be");
	__RESOURCE_EXCEPTION_IMPL(ResourceIndexOutOfRangeException,	"Resource data index was out of range");
	__RESOURCE_EXCEPTION_IMPL(ResourceSourceChangedException, "Resource source file has changed");
	__RESOURCE_EXCEPTION_IMPL(ResourceParseErrorException, "Parsing error");
	__RESOURCE_EXCEPTION_IMPL(ResourcePathTakenException, "Resource path was already taken");
	__RESOURCE_EXCEPTION_IMPL(ResourceFileMissingException, "File was expected to exist but it didn't");
	
	ResourceInvalidHandleException::ResourceInvalidHandleException(u32 id, u32 line, const string& fn)
		: ResourceException("", line, fn), m_id(id)
	{
	}

	string ResourceInvalidHandleException::GetMessage() const
	{
		return m_messagePrefix + " Invalid handle id " + std::to_string(m_id);
	}

	ResourceHandleRegisteredException::ResourceHandleRegisteredException(string existing, u32 id, u32 line, const string& fn)
		: ResourceException("", line, fn), msg("Handle with id '" + std::to_string(id) + "' already exists for " + existing)
	{
	}

	string ResourceHandleRegisteredException::GetSubMessage() const
	{
		return msg;
	}

	ResourceIsRuntimeException::ResourceIsRuntimeException(string context, u32 id, u32 line, const string& fn)
		: ResourceException("" + context, line, fn), msg("Handle with id '" + std::to_string(id) + "' is runtime. ")
	{
	}

	string ResourceIsRuntimeException::GetSubMessage() const
	{
		return msg;
	}

}