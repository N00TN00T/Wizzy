#include "spch.h"

#include <Wizzy.h>

struct RenderComponent
 	: public ecs::Component<RenderComponent> {
	wz::VertexArrayPtr va;
	wz::Material material;
};
struct TransformComponent
	: public ecs::Component<TransformComponent> {
	vec3 position;
	vec3 rotation;
	vec3 scale;

	inline mat4 ToMat4() const {
		mat4 _ret = glm::translate(glm::identity<mat4>(), position);
		_ret = glm::scale(_ret, scale);
		_ret = glm::rotate(_ret, rotation.x, vec3(1, 0, 0));
		_ret = glm::rotate(_ret, rotation.y, vec3(0, 1, 0));
		_ret = glm::rotate(_ret, rotation.z, vec3(0, 0, 1));

		return _ret;
	}
};
struct RenderEnvironmentComponent
	: public ecs::Component<RenderEnvironmentComponent> {
	mat4 view;
	mat4 projection;
};
struct TestTriangleComponent
	: public ecs::Component<TestTriangleComponent> {

};
struct PlayerTestTriangleComponent
	: public ecs::Component<PlayerTestTriangleComponent> {

};

class RendererSetupSystem
	: public ecs::System {
public:
	RendererSetupSystem() {
		AddComponentType<RenderEnvironmentComponent>();

		Subscribe((int32)wz::AppFrameBeginEvent::GetStaticType());
        Subscribe((int32)wz::AppUpdateEvent::GetStaticType());
		Subscribe((int32)wz::AppFrameEndEvent::GetStaticType());
        Subscribe((int32)wz::WindowResizeEvent::GetStaticType());
	}

	void BeginRenderer(const RenderEnvironmentComponent& env) const {
		wz::RenderCommand::Clear();
		wz::Renderer::Begin(env.view, env.projection);
	}

	void EndRenderer(const RenderEnvironmentComponent& env) const {
		wz::Renderer::End();
	}

    void OnWindowResize(const wz::WindowResizeEvent& e, RenderEnvironmentComponent& env) const {
        env.projection =
            glm::perspective<float>(wz::to_radians(90), e.GetX() / e.GetY(), .01f, 10000);
        wz::RenderCommand::SetViewport(0, 0, e.GetX(), e.GetY());
    }

    void OnUpdate(RenderEnvironmentComponent& env) const {
        ImGui::Begin("Camera");

        ImGui::DragFloat3("Position", glm::value_ptr(env.view) + 12, 0.05f);
        ImGui::DragFloat("FOV", glm::value_ptr(env.view) + 16, 0.01f);

        ImGui::End();
    }

	virtual
    void OnEvent(const void* eventHandle,
						 ecs::ComponentGroup& components) const override {
		const wz::Event& _e = *static_cast<const wz::Event*>(eventHandle);
		RenderEnvironmentComponent& _env = *components.Get<RenderEnvironmentComponent>();

		switch (_e.GetEventType()) {
			case wz::EventType::app_frame_begin:
				BeginRenderer(_env);
				break;
            case wz::EventType::app_update:
				OnUpdate(_env);
				break;
			case wz::EventType::app_frame_end:
				EndRenderer(_env);
				break;
            case wz::EventType::window_resize:
            {
                const wz::WindowResizeEvent& _windowResizeEvent =
                        *static_cast<const wz::WindowResizeEvent*>(eventHandle);
				OnWindowResize(_windowResizeEvent, _env);
				break;
            }
			default: break;
		}
	}
};

class RenderSystem
	: public ecs::System {
public:
	RenderSystem() {
		AddComponentType<RenderComponent>();
		AddComponentType<TransformComponent>();

		Subscribe((int32)wz::AppRenderEvent::GetStaticType());
	}

	void Submit(const RenderComponent& renderComponent,
				const TransformComponent& transformComponent) const {
		wz::Renderer::Submit(renderComponent.va, renderComponent.material,
						 transformComponent.ToMat4());
	}

	virtual void OnEvent(const void* eventHandle,
						 ecs::ComponentGroup& components) const override {
		const wz::Event& _e = *static_cast<const wz::Event*>(eventHandle);
		RenderComponent& _renderComponent =
								*components.Get<RenderComponent>();
		TransformComponent& _transformComponent =
								*components.Get<TransformComponent>();

		switch (_e.GetEventType()) {
			case wz::EventType::app_render:
				Submit(_renderComponent, _transformComponent);
				break;
			default: break;
		}
	}
};

class TestTriangleSystem
 	: public ecs::System{
public:
	TestTriangleSystem() {

		AddComponentType<TestTriangleComponent>();
		AddComponentType<TransformComponent>();
		AddComponentType<PlayerTestTriangleComponent>();

		Subscribe((int32)wz::AppUpdateEvent::GetStaticType());
		Subscribe((int32)wz::AppRenderEvent::GetStaticType());
	}

	void OnPlayerUpdate(TransformComponent& transform, float delta) const {
		if (wz::Input::GetKey(WZ_KEY_W)) {
			transform.position.y += delta * 100.f;
		}
		if (wz::Input::GetKey(WZ_KEY_A)) {
			transform.position.x -= delta * 100.f;
		}
		if (wz::Input::GetKey(WZ_KEY_S)) {
			transform.position.y -= delta * 100.f;
		}
		if (wz::Input::GetKey(WZ_KEY_D)) {
			transform.position.x += delta * 100.f;
		}
	}

	void OnRender(TransformComponent& transform) const {
		ImGui::Begin("Test triangle");

		ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.1f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 0.1f);
		ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.1f);

		ImGui::End();
	}

	virtual void OnEvent(const void* eventHandle,
						 ecs::ComponentGroup& components) const override {
		const wz::Event& _e = *static_cast<const wz::Event*>(eventHandle);
		TransformComponent& _transform =
								*components.Get<TransformComponent>();

		switch (_e.GetEventType()) {
			case wz::EventType::app_update:
			{
				const wz::AppUpdateEvent& _updateEv =
							*static_cast<const wz::AppUpdateEvent*>(eventHandle);
				if (components.Has<PlayerTestTriangleComponent>())
					OnPlayerUpdate(_transform, _updateEv.GetDeltaTime());
				break;
			}
			case wz::EventType::app_render:
				OnRender(_transform);
				break;
			default: break;
		}
	}
};

class Sandbox
	: public Wizzy::Application {
public:

	void CreateRendererManager() {
		RenderEnvironmentComponent _envComp;
		_envComp.view = glm::lookAt(vec3(0, 0, -10), vec3(0, 0, 0), vec3(0, 1, 0));
		_envComp.projection =
                        glm::perspectiveFov<float>(90,
                                                    m_window->GetWidth(),
                                                    m_window->GetHeight(),
                                                    .1f,
                                                    10000);
		ecs::IComponent* _comps[] = {
			&_envComp
		};
		ecs::StaticCId _ids[] = {
			_envComp.staticId
		};

		m_ecs.CreateEntity(_comps, _ids, 1);
	}

	void CreateTriangle(const wz::VertexArrayPtr& va, const wz::Material& mat, vec3 pos, vec3 rot, bool isPlayer = false) {
		RenderComponent _renderComp;
		_renderComp.va = va;
		_renderComp.material = mat;
		TransformComponent _transformComp;
		_transformComp.position = pos;
		_transformComp.rotation = rot;
		_transformComp.scale = vec3(1, 1, 1);
		TestTriangleComponent _triComp;
		PlayerTestTriangleComponent _playerComp;

		ecs::IComponent* _comps[] = {
			&_renderComp, &_transformComp, &_triComp, &_playerComp
		};
		ecs::StaticCId _ids[] = {
			_renderComp.staticId, _transformComp.staticId, _triComp.staticId,
			_playerComp.staticId
		};

		m_ecs.CreateEntity(_comps, _ids, isPlayer ? 4 : 3);
	}

	Sandbox() {

	}

	virtual void Init() override {

		wz::ResourceManagement::SetResourcePath(string(BASE_DIR) + "Resources/");

		wz::ResourceManagement::Import<wz::Shader> (
											"basic.shader",
											"basicShader"
										);
        wz::ResourceManagement::Import<wz::Script> (
											"test.lua",
											"testScript"
										);

		wz::Shader *_shader = wz::ResourceManagement::Get<wz::Shader>("basicShader");
		wz::VertexArrayPtr _vao(wz::VertexArray::Create());
		float _vertices[] = {
            // near bot left
			-0.5f, -0.5f, -0.5f, 		0.8f, 0.2f, 0.8f, 1.0f,
            // near bot right
			 0.5f, -0.5f, -0.5f, 		0.2f, 0.3f, 0.8f, 1.0f,
            // top
			 0.0f,  0.5f, 0.0f, 		0.8f, 0.8f, 0.2f, 1.0f,
             // far bot left
 			-0.5f, -0.5f, 0.5f, 		0.8f, 0.2f, 0.8f, 1.0f,
             // far bot right
 			 0.5f, -0.5f, 0.5f, 		0.2f, 0.3f, 0.8f, 1.0f,
		 };
		wz::VertexBufferPtr _vbo(wz::VertexBuffer::Create(_vertices, sizeof(_vertices)));

		wz::BufferLayout _layout = {
			{ wz::ShaderDataType::FLOAT3, "vertexPosition" },
			{ wz::ShaderDataType::FLOAT4, "vertexColor" }
		};
		_vbo->SetLayout(_layout);
		u32 _indices[] = {
            // near
            2, 1, 0,
            // right
            1, 2, 4,
            // far
            4, 2, 3,
            // left
            3, 2, 0,
            // bot - near left tri
            0, 1, 3,
            // bot - far right tri
            3, 1, 4,
        };
		wz::IndexBufferPtr _ibo(wz::IndexBuffer::Create(_indices, sizeof(_indices) / sizeof(u32)));
		_vao->PushVertexBuffer(_vbo);
		_vao->SetIndexBuffer(_ibo);

		CreateRendererManager();
		CreateTriangle(_vao, { _shader }, vec3(-1.5f, 0.f, 0.f), vec3(0, 0, 0));
		CreateTriangle(_vao, { _shader }, vec3(1.5f, 0.f, 0.f), vec3(0, 0, 0));
		CreateTriangle(_vao, { _shader }, vec3(0.f, 0.f, 0.f), vec3(0, 0, 0), true);

		m_gameSystems.AddSystem<RendererSetupSystem>();
		m_gameSystems.AddSystem<TestTriangleSystem>();
		m_gameSystems.AddSystem<RenderSystem>();
		wz::RenderCommand::SetClearColor(.1f, .2f, .5f, 1.f);
	}

};

Wizzy::Application* CreateApplication() {
    return new Sandbox();
}
