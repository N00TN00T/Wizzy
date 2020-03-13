#include "spch.h"

#include <Wizzy.h>

#include "Systems.h"
#include "ResourceList.h"
#include "..\SceneSystem\Systems.h"

wz::RenderTarget::Handle hMainRenderTarget = WZ_NULL_RESOURCE_HANDLE;

ResourceManager::ResourceManager()
{
	AddComponentType<ResourceManagerComponent>();

	this->Subscribe(wz::EventType::app_init);
	this->Subscribe(wz::EventType::app_update);
	this->Subscribe(wz::EventType::app_shutdown);
}

void ResourceManager::OnEvent(const Wizzy::Event& e, Wizzy::ComponentGroup& components) const
{
	auto& data = *components.Get<ResourceManagerComponent>();

	switch (e.GetEventType())
	{
		case wz::EventType::app_init:
		{
			wz::ResourceManagement::SetResourceDir(data.resourceDir);
			LoadResourceList("ResourceList.rl");
			return;
		}
		case wz::EventType::app_update:
		{
			wz::ResourceManagement::SetResourceDir(data.resourceDir);

			auto& event = (wz::AppUpdateEvent&)e;

			data.timeSinceValidate += event.GetDeltaTime();

			if (data.timeSinceValidate >= data.validationRate / 1.f)
			{
				data.timeSinceValidate = 0;

				static std::future<void> future;
				static bool first = true;
				if (first || future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
				{
					future = std::async(std::launch::async, []() { wz::ResourceManagement::Validate(true); });
					first = false;
				}
			}

			return;
		}
		case wz::EventType::app_shutdown:
		{
			WriteResourceList("ResourceList.rl");
		}
	}
}




TextureRenderer::TextureRenderer()
{
	AddComponentType<TextureComponent>();
	AddComponentType<TransformComponent>();

	Subscribe(wz::EventType::app_render);
}

void TextureRenderer::OnEvent(const Wizzy::Event& e, Wizzy::ComponentGroup& components) const
{
	WZ_TRACE("Submitting a texture from texture renderer...");
	auto& textureData = *components.Get<TextureComponent>();
	auto& transform = *components.Get<TransformComponent>();

	try
	{
		if (wz::ResourceManagement::IsLoaded(textureData.hTexture))
		{
			wz::Renderer2D::SubmitImage(textureData.hTexture, transform.position, transform.scale, transform.rotation, textureData.color, hMainRenderTarget);
		}
		else
		{
			wz::Renderer2D::SubmitRect(wz::Rect(transform.position, 64.f * transform.scale), wz::Color::red, Wizzy::RectMode::Filled);
		}
	}
	catch (const wz::RendererException& e)
	{
		/*WZ_CORE_WARN("Failed submitting texture in TextureRenderer:");
		WZ_CORE_WARN("{0}", e.GetMessage());*/
	}
}




RendererManager::RendererManager()
{
	AddComponentType<RendererManagerComponent>();

	Subscribe(wz::EventType::app_init);
	Subscribe(wz::EventType::app_frame_begin);
	Subscribe(wz::EventType::app_update);
	Subscribe(wz::EventType::app_frame_end);
	Subscribe(wz::EventType::window_resize);
}

void RendererManager::OnEvent(const Wizzy::Event& e, Wizzy::ComponentGroup& components) const
{
	auto& config = *components.Get<RendererManagerComponent>();

	switch (e.GetEventType())
	{
		case wz::EventType::app_init:
		{
			wz::RenderCommand::SetClearColor(wz::Color::gray);
			wz::RenderCommand::SetCullMode(wz::WZ_CULL_NONE);
			wz::RenderCommand::ToggleBlending(true);
			auto& window = wz::Application::Get().GetWindow();
			//wz::RenderCommand::SetViewport(wz::Viewport(0, 0, window.GetWidth(), window.GetHeight()));
			wz::RenderCommand::ToggleDepthTesting(false);
			window.SetVSync(false);
			return;
		}
		case wz::EventType::app_update:
		{
			ImGui::DockSpaceOverViewport();
			
			return;
		}
		case wz::EventType::app_frame_begin:
		{
			wz::RenderCommand::Clear();
			return;
		}
		case wz::EventType::app_frame_end:
		{
			
			
			
			return;





				// Move rendering to cameras to render directly to their rendertarget
				// If they are the main camera, also render its rendertarget to the main window






		}
		case wz::EventType::window_resize:
		{
			auto& window = wz::Application::Get().GetWindow();
			//wz::RenderCommand::SetViewport(wz::Viewport(0, 0, window.GetWidth(), window.GetHeight()));
			return;
		}
	}
}




CameraSystem::CameraSystem()
{
	AddComponentType<TransformComponent>();
	AddComponentType<CameraComponent>();

	Subscribe(wz::EventType::app_init);
	Subscribe(wz::EventType::app_frame_begin);
	Subscribe(wz::EventType::app_update);
	Subscribe(wz::EventType::app_frame_end);
	Subscribe(wz::EventType::app_render);
	Subscribe(wz::EventType::window_resize);
}

void CameraSystem::OnEvent(const Wizzy::Event& e, Wizzy::ComponentGroup& components) const
{
	auto& transform = *components.Get<TransformComponent>();
	auto& camera = *components.Get<CameraComponent>();

	switch (e.GetEventType())
	{
		case wz::EventType::app_init:
		{
			auto& window = wz::Application::Get().GetWindow();
			camera.projection = glm::ortho<float>(0, window.GetWidth(), 0, window.GetHeight(), -1, 1);

			//camera.hRenderTarget = wz::ResourceManagement::AddResource(wz::RenderTarget::Create(1600, 900), "testing/main.rt", wz::RenderTarget::GetTemplateProps());
			camera.hRenderTarget = (wz::RenderTarget::Handle)wz::ResourceManagement::HandleOf("testing/main.rt");

			if (!wz::ResourceManagement::IsLoaded(hMainRenderTarget) && wz::ResourceManagement::IsLoaded(camera.hRenderTarget))
			{
				hMainRenderTarget = camera.hRenderTarget;
			}
			return;
		}
		case wz::EventType::app_frame_begin:
		{
			return;
		}
		case wz::EventType::app_update:
		{
			auto& eUpdate = (const wz::AppUpdateEvent&)e;
			camera.transform = glm::translate(mat4(1.f), vec3(transform.position + vec2(800, 450), -.5f));
			camera.transform = glm::scale(camera.transform, vec3(transform.scale, 1.f));
			camera.transform = glm::rotate(camera.transform, glm::radians(transform.rotation), vec3(0, 0, 1));
			camera.transform = glm::translate(camera.transform, vec3(-vec2(800, 450), 0.f));
			camera.transform = glm::inverse(camera.transform);
			
			auto speed = 1000.f;
			if (wz::Input::GetKey(WZ_KEY_W))
			{
				transform.position.y += speed * eUpdate.GetDeltaTime();
			}
			if (wz::Input::GetKey(WZ_KEY_A))
			{
				transform.position.x -= speed * eUpdate.GetDeltaTime();
			}
			if (wz::Input::GetKey(WZ_KEY_S))
			{
				transform.position.y -= speed * eUpdate.GetDeltaTime();
			}
			if (wz::Input::GetKey(WZ_KEY_D))
			{
				transform.position.x += speed * eUpdate.GetDeltaTime();
			}

			if (wz::Input::GetKey(WZ_KEY_E))
			{
				transform.scale.x += 1.f * eUpdate.GetDeltaTime();
				transform.scale.y += 1.f * eUpdate.GetDeltaTime();
			}

			if (wz::Input::GetKey(WZ_KEY_Q))
			{
				transform.scale.x -= 1.f * eUpdate.GetDeltaTime();
				transform.scale.y -= 1.f * eUpdate.GetDeltaTime();
			}

			
			return;
		}
		case wz::EventType::app_render:
		{
			try
			{
				wz::Renderer2D::Begin(camera.hShader, camera.projection * camera.transform, camera.hRenderTarget);
				camera.rendererReady = true;
			}
			catch (const wz::RendererException e)
			{
				WZ_CORE_WARN("Error when calling Renderer Begin() on camera:");
				WZ_CORE_WARN("{0}", e.GetMessage());
				camera.rendererReady = false;
			}
			return;
		}
		case wz::EventType::app_frame_end:
		{
			if (camera.rendererReady)
			{
				try
				{
					wz::Renderer2D::End(camera.hRenderTarget);
				}
				catch (const wz::RendererException e)
				{
					WZ_CORE_WARN("Error when calling renderer end on camera:");
					WZ_CORE_WARN("{0}", e.GetMessage());
				}
			}

			if (hMainRenderTarget == camera.hRenderTarget)
			{
				ImGui::Begin("Game", NULL, ImGuiWindowFlags_MenuBar);

				ImGui::BeginMenuBar();
				float menuBarHeight = ImGui::GetFrameHeight() * 2;
				static string txt = std::to_string(wz::Application::Get().GetWindow().GetDeltaTime());
				if (rand() % 100 == 3) txt = std::to_string(wz::Application::Get().GetWindow().GetDeltaTime());
				ImGui::Text(txt.c_str());
				ImGui::EndMenuBar();

				float a = ImGui::GetWindowSize().x / ImGui::GetWindowSize().y;
				float ySize = ImGui::GetWindowSize().y - menuBarHeight;

				wz::RenderTarget* mainRenderTarget = NULL;
				if (wz::ResourceManagement::TryGet(hMainRenderTarget, mainRenderTarget))
				{
					ImGui::Image((ImTextureID)mainRenderTarget->GetTextureId(), { ySize * a, ySize }, { 0, 1 }, { 1, 0 });
				}
				
				ImGui::End();
			}
			return;
		}
		case wz::EventType::window_resize:
		{
			auto& ev = (const wz::WindowResizeEvent&)e;
			camera.projection = glm::ortho<float>(0, ev.GetX(), 0, ev.GetY(), -1, 1);
			return;
		}
	}
}
