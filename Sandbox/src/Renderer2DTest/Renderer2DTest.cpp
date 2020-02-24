#include "spch.h"

#include "Renderer2DTest.h"


#define LOAD 1


void Renderer2DTest::Init()
{
	wz::Log::SetCoreLogLevel(LOG_LEVEL_TRACE);
#if LOAD
	m_clientEcs.Load("test.ecs");
#else
	m_clientEcs.CreateEntity<ResourceManagerComponent>();
	RendererManagerComponent rmc;
	m_clientEcs.CreateEntity(rmc);

	CameraComponent cc;
	cc.hShader = 2ULL;
	m_clientEcs.CreateEntity(cc, TransformComponent());
	
	for (int i = 0; i < 30000; i++)
	{
		TextureComponent tc;
		TransformComponent trc;
		tc.hTexture = 3;
		trc.position = { rand() % 10000 - 5000, rand() % 10000 - 5000 };
		m_clientEcs.CreateEntity(tc, trc);
	}

#endif

	m_clientSystems.AddSystem<ResourceManager>();
	m_clientSystems.AddSystem<RendererManager>();
	m_clientSystems.AddSystem<CameraSystem>();

	m_clientSystems.AddSystem<TextureRenderer>();
}

void Renderer2DTest::Shutdown()
{
	m_clientEcs.Save("test.ecs");
}
