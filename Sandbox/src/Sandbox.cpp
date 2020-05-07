#include "spch.h"

#include "Sandbox.h"

#define str(x) std::to_string(x)

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

	AddEventCallback(WZ_BIND_FN(Sandbox::OnEvent));

	wz::ResourceManagement::SetResourceDir(
		ulib::File::directory_of(GetExecutablePath()) + "/../../../res/");

	texture = wz::ResourceManagement::Load<wz::Texture>("Wizzy.png");
	font = wz::ResourceManagement::Load<wz::Font>("InputMono-Black.ttf");
	//song = wz::ResourceManagement::Load<wz::AudioClip>("song.wav");

	pipeline = new wz::Renderer2D::Pipeline(_MB(15));
	pipeline->camTransform = glm::ortho<float>(0, 1600, 0, 900);

	textPipeline = new wz::Renderer2D::Pipeline(_MB(5));
	textPipeline->camTransform = glm::ortho<float>(0, 1600, 0, 900);
	textPipeline->hShader = wz::Renderer2D::GetTextShader();

	wz::RenderCommand::SetClearColor(wz::Color::blue);

	#ifndef WZ_CONFIG_DEBUG
	sizex = sizey = 3.8f;
	#endif

	return;
}


void Sandbox::Shutdown()
{
}

void Sandbox::OnEvent(wz::Event& e)
{
	wz::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<wz::AppUpdateEvent>(WZ_BIND_FN(Sandbox::Update));
	dispatcher.Dispatch<wz::AppRenderEvent>(WZ_BIND_FN(Sandbox::Render));
}

bool Sandbox::Update(wz::AppUpdateEvent& e)
{
	try
	{
		wz::ResourceManagement::Validate();
	}
	catch(const wz::ResourceException& e)
	{
		WZ_CORE_CRITICAL("Resource validation failed:\n", e.GetMessage());
		WZ_BREAK;
	}
	

	frameTime = ((const wz::AppUpdateEvent&)e).GetDeltaTime();

	return false;
}

bool Sandbox::Render(wz::AppRenderEvent& e)
{
	wz::Renderer2D::Clear(pipeline);
	wz::Renderer2D::ResetMetrics();
	wz::Renderer2D::Begin(pipeline);
	
	for (float x = 0.f; x < 1600.f; x += sizex)
	{
		for (float y = 0.f; y < 900.f; y += sizey)
		{
			if (!textured)
			{
				wz::Renderer2D::SubmitRect
				(
					pipeline,
					wz::Rect(x, y, sizex, sizey),
					wz::Color(0.f, x / 1600.f, y / 900.f, 1.f)
				);
			}
			else
			{
				wz::Renderer2D::SubmitTexture
				(
					pipeline,
					texture,
					{ x, y },
					{ sizex / 16.f, sizey / 16.f },
					0,
					wz::Color(1.f, x / 1600.f, y / 900.f, 1.f)
				);
			}
		}
	}

	const auto& metrics = wz::Renderer2D::GetMetrics();

	

	wz::Renderer2D::End(pipeline);

	wz::Renderer2D::Begin(textPipeline);

	if (text)
	{
		wz::Renderer2D::SubmitText
		(
			textPipeline, 
			font,
			"I am very capable of rendering text\nI also support rendering strings with newlines (\\n) like this\nHowever, I sadly do not support unicode characters yet: äåöäåö",
			{ 0, 150 }
		);

		wz::Renderer2D::SubmitText
		(
			textPipeline, 
			font,
			"These texts are actually render targets that were rendered once\nSo static text costs just as much as rendering textures\nHowever, when cached texts exceeds the budget, there is a very \nnoticeable performance hit.",
			{ 0, 0 },
			{ 1.f, 1.f },
			0,
			wz::Color::red
		);

		vec2 pos = { 50.f, m_window->GetHeight() - 8 * 36 }; 
		string metricsStr = str(metrics.numQuads) + " Quads\n"
							+ str(metrics.numIndices) + " Indices\n"
							+ str(metrics.GetNumVertices()) + " Vertices\n"
							+ str(metrics.numDrawCalls + 1) + " Draw Calls\n"
							+ str(metrics.GetMemoryUsage()) + " Memory Usage (bytes)\n"
							//+ str(frameTime) + " Frame time (ms)\n"
							//+ str(1.f / frameTime) + " FPS\n"
							;
		wz::Renderer2D::SubmitText(textPipeline, font, metricsStr, pos);
	}

	wz::Renderer2D::End(textPipeline);

	ImGui::Begin("Debug");
	ImGui::Checkbox("Textured quads", &textured);
	ImGui::DragFloat2("Quad size", &sizex, .01f, 1.f, 100.f);
	ImGui::Checkbox("Render text", &text);
	ImGui::Text("%f Frame time (ms)", frameTime);
	ImGui::Text("%f FPS", 1.f / frameTime);
	if (!text)
	{
		ImGui::Text("%i Quads", metrics.numQuads);
		ImGui::Text("%i Indices", metrics.numIndices);
		ImGui::Text("%i Vertices", metrics.GetNumVertices());
		ImGui::Text("%i Draw Calls", metrics.numDrawCalls);
		ImGui::Text("%i Memory Usage (bytes)", metrics.GetMemoryUsage());
	}

	ImGui::End();

	return false;
}
