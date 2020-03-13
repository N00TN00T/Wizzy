#include "spch.h"

#include "SceneSystem.h"

#include "../Renderer2DTest/Systems.h"
#include "Scene.h"
#include "ResourceList.h"
#include "Systems.h"

struct TestComponent : public wz::Component<TestComponent>
{
	char text[128] = "Hiiiiiiiiiiiii";
	int integer = 5;
	float decimal = 420.69f;
	bool boolean = false;
};

void SceneSystem::Init()
{
	wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);

	wz::ResourceManagement::SetResourceDir("../res/");
	LoadResourceList("ResourceList.rl");

	m_clientEcs.CreateEntity<SceneComponent>();

	m_clientSystems.AddSystem<SceneManager>();
}

void SceneSystem::Shutdown()
{
	WriteResourceList("ResourceList.rl");
}
