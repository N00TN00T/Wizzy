#pragma once

#include <Wizzy.h>
#include "SceneSystem/Scene.h"

inline void LoadResourceList(const string& file)
{
	string fullPath = wz::ResourceManagement::GetResourceDir() + file;

	string data = "";
	if (!ulib::File::read(fullPath, &data, true))
	{
		WZ_THROW(ResourceFileAccessException, fullPath);
	}

	std::stringstream s(data);
	string line;
	while (std::getline(s, line))
	{
		if (line.find(';') == string::npos) WZ_THROW(ResourceParseErrorException, file);
		string resPath = line.substr(0, line.find_first_of(';'));
		line = line.substr(line.find_first_of(';') + 1);
		string typeStr = line.substr(0, line.find_first_of(';'));
		line = line.substr(line.find_first_of(';') + 1);
		string idStr = line;
		wz::uId id = 0;
		try
		{
			id = std::stoi(idStr);
		}
		catch (const std::exception & e)
		{
			WZ_THROW(Exception, "Could not convert string '" + idStr + "' to int: " + string(e.what()));
		}

		try
		{
			if (typeStr == typestr(wz::Script))
			{
				wz::ResourceManagement::Load<wz::Script>(resPath, id);
			}
			else if (typeStr == typestr(wz::Shader))
			{
				wz::ResourceManagement::Load<wz::Shader>(resPath, id);
			}
			else if (typeStr == typestr(wz::Material))
			{
				wz::ResourceManagement::Load<wz::Material>(resPath, id);
			}
			else if (typeStr == typestr(wz::Texture))
			{
				wz::ResourceManagement::Load<wz::Texture>(resPath, id);
			}
			else if (typeStr == typestr(wz::RenderTarget))
			{
				wz::ResourceManagement::Load<wz::RenderTarget>(resPath, id);
			}
			else if (typeStr == typestr(wz::Font))
			{
				wz::ResourceManagement::Load<wz::Font>(resPath, id);
			}
			else if (typeStr == typestr(Scene))
			{
				wz::ResourceManagement::Load<Scene>(resPath, id);
			}
			else
			{
				WZ_THROW(Exception, "Unhandled or invalid type string: " + typeStr);
			}
		}
		catch (const wz::Exception & e)
		{
			WZ_ERROR("Exception when loading from resource list");
			WZ_ERROR(e.GetUnhandledMessage());
		}
	}

	WZ_INFO("Successfully loaded Resource List!");
}

inline void WriteResourceList(const string& file)
{
	string list = "";

	for (auto handle : wz::ResourceManagement::GetHandles())
	{
		auto& info = wz::ResourceManagement::GetInfoFor(handle);
		if (info.runtime) continue;

		list += info.resPath + ";" + info.type + ";" + std::to_string(handle.id) + "\n";
	}

	if (!ulib::File::write(wz::ResourceManagement::GetResourceDir() + file, list, true))
	{
		WZ_THROW(ResourceFileAccessException, wz::ResourceManagement::GetResourceDir() + file);
	}

	for (auto& hnd : wz::ResourceManagement::GetHandles())
	{
		auto& info = wz::ResourceManagement::GetInfoFor(hnd);
		if (info.runtime) continue;
		if (wz::ResourceManagement::IsValid(hnd) && wz::ResourceManagement::IsLoaded(hnd))
		{
			wz::ResourceManagement::Save(hnd);
		}
	}
}