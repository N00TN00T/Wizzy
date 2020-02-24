#include "spch.h"

#include "NodeScripting.h"

#include "../Renderer2DTest/Systems.h"

#define LOAD 0

void NodeScripting::Init()
{
	wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);
#if LOAD
	m_clientEcs.Load("test.ecs");
#else
	m_clientEcs.CreateEntity<ResourceManagerComponent>();
	RendererManagerComponent rmc;
	m_clientEcs.CreateEntity(rmc);

	CameraComponent cc;
	cc.hShader = 2ULL;
	m_clientEcs.CreateEntity(cc, TransformComponent());

	/*for (int i = 0; i < 30000; i++)
	{
		TextureComponent tc;
		TransformComponent trc;
		tc.hTexture = 3;
		trc.position = { rand() % 10000 - 5000, rand() % 10000 - 5000 };
		m_clientEcs.CreateEntity(tc, trc);
	}*/

	TextureComponent tc;
	TransformComponent trc;
	tc.hTexture = 3;
	trc.position = { 0, 0 };
	m_clientEcs.CreateEntity(tc, trc);

#endif

	m_clientSystems.AddSystem<ResourceManager>();
	m_clientSystems.AddSystem<RendererManager>();
	m_clientSystems.AddSystem<CameraSystem>();

	m_clientSystems.AddSystem<TextureRenderer>();
}

void NodeScripting::Shutdown()
{
	m_clientEcs.Save("nodescripting.ecs");
}
