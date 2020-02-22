#include "spch.h"

#include "Renderer2DTest.h"





void Renderer2DTest::Init()
{
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

	m_clientSystems.AddSystem<ResourceManager>();
	m_clientSystems.AddSystem<RendererManager>();
	m_clientSystems.AddSystem<CameraSystem>();

	m_clientSystems.AddSystem<TextureRenderer>();
}

void Renderer2DTest::Shutdown()
{
}
