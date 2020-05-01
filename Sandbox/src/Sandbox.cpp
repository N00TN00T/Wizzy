#include "spch.h"

#include "Sandbox.h"

struct TestComponent : public wz::Component<TestComponent>
{
	wz::Texture::Handle texture;
	wz::Font::Handle font;
	wz::Renderer2D::Pipeline* pipeline;
	float frameTime = 0;
	bool textured = false;
	bool text = true;
	float sizex = 30.f;
	float sizey = 30.f;
};

class TestSystem : public wz::System
{
public:
	TestSystem()
	{
		AddComponentType<TestComponent>();

		Subscribe(wz::EventType::app_init);
		Subscribe(wz::EventType::app_update);
		Subscribe(wz::EventType::app_render);
	}

	virtual void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const override
	{
		auto& comp = *components.Get<TestComponent>();
		switch (e.GetEventType())
		{
			case wz::EventType::app_init:
			{	
				wz::ResourceManagement::SetResourceDir("../res/");

				comp.texture = wz::ResourceManagement::Load<wz::Texture>("Wizzy.png");
				comp.font = wz::ResourceManagement::Load<wz::Font>("InputMono-Black.ttf");

				comp.pipeline = new wz::Renderer2D::Pipeline(_MB(15));
				comp.pipeline->camTransform = glm::ortho<float>(0, 1600, 0, 900);

				wz::RenderCommand::SetClearColor(wz::Color::blue);
				return;
			}
			case wz::EventType::app_update:
			{
				wz::Renderer2D::Clear(comp.pipeline);
				comp.frameTime = ((const wz::AppUpdateEvent&)e).GetDeltaTime();
				return;
			}
			case wz::EventType::app_render:
			{
				wz::Renderer2D::ResetMetrics();

				wz::Renderer2D::Begin(comp.pipeline);
				
				for (float x = 0.f; x < 1600.f; x += comp.sizex)
				{
					for (float y = 0.f; y < 900.f; y += comp.sizey)
					{
						if (!comp.textured)
						{
							wz::Renderer2D::SubmitRect
							(
								comp.pipeline,
								wz::Rect(x, y, comp.sizex, comp.sizey),
								wz::Color(0.f, x / 1600.f, y / 900.f, 1.f)
							);
						}
						else
						{
							wz::Renderer2D::SubmitTexture
							(
								comp.pipeline,
								comp.texture,
								{ x, y },
								{ comp.sizex / 16.f, comp.sizey / 16.f },
								0,
								wz::Color(1.f, x / 1600.f, y / 900.f, 1.f)
							);
						}
					}
				}

				if (comp.text)
				{
					wz::Renderer2D::SubmitText
					(
						comp.pipeline, 
						comp.font,
						"I am very capable of rendering text\nI also support rendering strings with newlines (\\n) like this\nHowever, I sadly do not support unicode characters yet: äåöäåö",
						{ 0, 100 },
						{ 1.f, 1.f },
						0,
						wz::Color::white
					);

					wz::Renderer2D::SubmitText
					(
						comp.pipeline, 
						comp.font,
						"These texts are actually render targets that were rendered once\nSo it's pretty much 0 costs (until I render 100+ different texts)",
						{ 0, 0 },
						{ 1.f, 1.f },
						0,
						wz::Color::red
					);
				}

				wz::Renderer2D::End(comp.pipeline);

				const auto& metrics = wz::Renderer2D::GetMetrics();

				ImGui::Begin("Renderer2D Metrics");
				ImGui::Text("%i Quads", metrics.numQuads);
				ImGui::Text("%i Indices", metrics.numIndices);
				ImGui::Text("%i vertices", metrics.GetNumVertices());
				ImGui::Text("%i Draw Calls", metrics.numDrawCalls);
				ImGui::Text("%i Memory usage (bytes)", metrics.GetMemoryUsage());
				ImGui::Text("%f Frame time (ms)", comp.frameTime);
				ImGui::Text("%.1f FPS", 1.f / comp.frameTime);
				ImGui::Checkbox("Textured quads", &comp.textured);
				ImGui::DragFloat2("Quad size", &comp.sizex, .01f, 1.f, 100.f);
				ImGui::Checkbox("Render text", &comp.text);
				ImGui::End();

				return;
			}
		}
	}

};

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
}

void Sandbox::Init()
{
	wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);
	wz::Log::SetClientLogLevel(LOG_LEVEL_TRACE);

	srand(time(NULL));

	m_clientEcs.CreateEntity<TestComponent>();

	m_clientSystems.AddSystem<TestSystem>();

}


void Sandbox::Shutdown()
{
}
