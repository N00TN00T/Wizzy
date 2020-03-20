#include "spch.h"

#include "Renderer2DTest.h"


#define LOAD 0


void Renderer2DTest::Init()
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
	
	for (int i = 0; i < 300; i++)
	{
		TextureComponent tc;
		TransformComponent trc;

		if (rand() % 2 == 1)
		{
			tc.hTexture = 3;
		}
		else
		{
			tc.hTexture = WZ_NULL_RESOURCE_HANDLE;
		}
		trc.position = { rand() % 10000 - 5000, rand() % 10000 - 5000 };
		m_clientEcs.CreateEntity(tc, trc);
	}

	m_clientEcs.CreateEntity<TransformComponent, LabelComponent>();

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
