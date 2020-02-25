#include "spch.h"

#include "ProjectManagement.h"

#define PATH_NAME_PROJECT_DATA_DIRECTORY		string("data")
#define PATH_NAME_PROJECT_RESOURCES_DIRECTORY	string("res")
#define PATH_NAME_PROJECT_FILE					string("wzproject")
#define PATH_NAME_PROJECT_ECS_FILE				string("project.ecs")

ProjectManagement::Project* ProjectManagement::s_project = NULL;

status ProjectManagement::CreateProject(const string& directory, const string& name)
{
	string projectFile = directory + "/" + PATH_NAME_PROJECT_FILE;
	if (ulib::File::exists(projectFile))
	{
		return CODE_PROJECT_EXISTS;
	}

	if (!ulib::File::write(projectFile, name)) return CODE_FILE_INACCESSIBLE;

	if (!ulib::File::exists(directory))
	{
		if (!ulib::Directory::create_all(directory)) return CODE_FILE_INACCESSIBLE;
	}
	string dataDirectory = directory + "/" + PATH_NAME_PROJECT_DATA_DIRECTORY;
	if (!ulib::File::exists(dataDirectory))
	{
		if (!ulib::Directory::create(dataDirectory)) return CODE_FILE_INACCESSIBLE;
	}
	string resDirectory = directory + "/" + PATH_NAME_PROJECT_RESOURCES_DIRECTORY;
	if (!ulib::File::exists(resDirectory))
	{
		if (!ulib::Directory::create(resDirectory)) return CODE_FILE_INACCESSIBLE;
	}

	Project* dummyProject = new Project();
	dummyProject->name = name;
	dummyProject->projectDirectory = directory;
	dummyProject->ecs.Save(dataDirectory + "/" + PATH_NAME_PROJECT_ECS_FILE);
	dummyProject->valid = true;
	if (!ValidateProject(dummyProject))
	{
		delete dummyProject;
		return CODE_CREATE_FAILURE;
	}
	
	delete dummyProject;
	return CODE_OK;
}

status ProjectManagement::LoadProject(const string& path)
{
	Project* newProject = new Project();
	newProject->valid = true;
	if (ulib::File::name_of(path) == PATH_NAME_PROJECT_FILE)
	{
		newProject->projectDirectory = ulib::File::directory_of(path);
	}
	else if (ulib::File::exists(path + "/" + PATH_NAME_PROJECT_FILE))
	{
		newProject->projectDirectory = path;
	}
	else
	{
		return CODE_INVALID_DIRECTORY;
	}

	if (!ValidateProject(newProject))
	{
		return CODE_INVALID_DIRECTORY;
	}

	if (!ulib::File::read(ProjectFile(newProject), &newProject->name)) return CODE_FILE_INACCESSIBLE;

	newProject->ecs.Load(ProjectEcsFile(newProject));

	if (s_project) delete s_project;
	s_project = newProject;

	return CODE_OK;
}

status ProjectManagement::SaveProject()
{
	if (!ValidateProject(s_project))
	{
		return CODE_INVALID_PROJECT;
	}

	if (!ulib::File::write(ProjectFile(s_project), s_project->name)) return CODE_FILE_INACCESSIBLE;
	s_project->ecs.Save(ProjectEcsFile(s_project));

	return CODE_OK;
}

string ProjectManagement::ProjectEcsFile(Project* p)
{
	return DataDirectory(p) + "/" + PATH_NAME_PROJECT_ECS_FILE;
}

string ProjectManagement::DataDirectory(Project* p)
{
	return p->projectDirectory + "/" + PATH_NAME_PROJECT_DATA_DIRECTORY;
}

string ProjectManagement::ResourceDirectory(Project* p)
{
	return p->projectDirectory + "/" + PATH_NAME_PROJECT_RESOURCES_DIRECTORY;
}

string ProjectManagement::ProjectFile(Project* p)
{
	return p->projectDirectory + "/" + PATH_NAME_PROJECT_FILE;
}

string ProjectManagement::ProjectDirectory(Project* p)
{
	return p->projectDirectory;
}

bool ProjectManagement::ValidateProject(Project* p)
{
	WZ_CORE_DEBUG("ProjectEcsFile {0}, {1}", !ulib::File::exists(ProjectEcsFile(p)), ProjectEcsFile(p));
	WZ_CORE_DEBUG("ResourceDirectory {0}, {1}", !ulib::Directory::exists(ResourceDirectory(p)), ResourceDirectory(p));
	WZ_CORE_DEBUG("ProjectFile {0}, {1}", !ulib::File::exists(ProjectFile(p)), ProjectFile(p));
	if (p == NULL
		|| !ulib::File::exists(ProjectEcsFile(p))
		|| !ulib::Directory::exists(ResourceDirectory(p))
		|| !ulib::File::exists(ProjectFile(p))
		|| !p->valid)
	{
		return false;
	}

	return true;
}
