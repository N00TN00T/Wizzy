#pragma once

#include <Wizzy.h>


typedef u8 status;
class ProjectManagement
{
private:
	struct Project;
public:
	static status CreateProject(const string& directory, const string& name);
	static status LoadProject(const string& path);
	static status SaveProject();

	inline static Project* GetProject() { return s_project; }
	inline static wz::ECSManager& GetEcs() { return s_project->ecs; };

private:
	static string ProjectEcsFile(Project* p);
	static string DataDirectory(Project* p);
	static string ResourceDirectory(Project* p);
	static string ProjectFile(Project* p);
	static string ProjectDirectory(Project* p);

	static bool ValidateProject(Project* p);

private:
	struct Project
	{
		string name = "UnnamedProject";
		string projectDirectory;
		wz::ECSManager ecs;
		bool valid = false;
	};
	static Project* s_project;

public:
	static constexpr status CODE_OK					= 0; // No problems
	static constexpr status CODE_PROJECT_EXISTS		= 1; // Tried creating project where a project already exists
	static constexpr status CODE_FILE_INACCESSIBLE	= 2; // Failed accessing some file
	static constexpr status CODE_INVALID_DIRECTORY	= 3; // Target directory for loading is not a valid project directory
	static constexpr status CODE_INVALID_PROJECT	= 4; // Current project is not valid and can't be saved
	static constexpr status CODE_CREATE_FAILURE		= 5; // Error when creating a project
};	