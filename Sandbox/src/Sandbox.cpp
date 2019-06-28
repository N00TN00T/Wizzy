#include "spch.h"

#include <Wizzy.h>

struct RenderComponent
 	: public ecs::Component<RenderComponent> {
private:
    static u32 renderIdCounter;
public:
	wz::VertexArrayPtr va;
	wz::Material material;
    u32 renderId = ++renderIdCounter;
    string shaderPreview;
    string texturePreview;
    string diffuseModePreview;

    RenderComponent(const wz::VertexArrayPtr& va, const wz::Material& material)
        : va(va), material(material), shaderPreview(""), texturePreview(""), diffuseModePreview("Texture") {
        if (!material.diffuseTexture)
            diffuseModePreview = "Color";
    }
};
u32 RenderComponent::renderIdCounter = 0;

struct TransformComponent
	: public ecs::Component<TransformComponent> {
	vec3 position;
	vec3 rotation;
	vec3 scale;

	inline mat4 ToMat4() const {
		mat4 _ret = glm::translate(glm::identity<mat4>(), position);
		_ret = glm::rotate(_ret, rotation.x, vec3(1, 0, 0));
		_ret = glm::rotate(_ret, rotation.y, vec3(0, 1, 0));
		_ret = glm::rotate(_ret, rotation.z, vec3(0, 0, 1));
        _ret = glm::scale(_ret, scale);

		return _ret;
	}

};

struct TestTriangleComponent
	: public ecs::Component<TestTriangleComponent> {

};
struct PlayerTestTriangleComponent
	: public ecs::Component<PlayerTestTriangleComponent> {

};

struct ViewComponent
    : public ecs::Component<ViewComponent> {
    //view
    vec3 position;
    vec3 rotation;

    //projection
    float fov;
    float aspectRatio;
    float nearClip;
    float farClip;
    bool dynamicAspectRatio = false;

    mat4 ToMat4() {
        mat4 _view = glm::identity<mat4>();

        _view = glm::rotate(_view, rotation.x, vec3(1, 0, 0));
        _view = glm::rotate(_view, rotation.y, vec3(0, 1, 0));
        _view = glm::rotate(_view, rotation.z, vec3(0, 0, 1));
        _view = glm::translate(_view, position);

        mat4 _projection = glm::identity<mat4>();

        _projection = glm::perspectiveFov<double>(wz::to_radians(fov),
                                                 1.0 * aspectRatio,
                                                 1.0,
                                                 nearClip,
                                                 farClip);

        return _projection * _view;
    }
};

struct ResourceGUIComponent
    : public ecs::Component<ResourceGUIComponent> {

};

class ResourceGUISystem
    : public ecs::System {
public:
    ResourceGUISystem() {
        AddComponentType<ResourceGUIComponent>();

        Subscribe((int)wz::EventType::app_render);
    }

    void SubmitGUI() const {
        ImGui::Begin("Resources");

        for (const auto& _alias : wz::ResourceManagement::GetAliases()) {
            auto _resource = wz::ResourceManagement::Get<wz::IResource>(_alias);
            string _headerStr = _alias + " : " + _resource->GetType();
            ImGui::Text(_headerStr.c_str());
            ImGui::Spacing();
        }

        ImGui::End();
    }

    virtual void OnEvent(const void* eventHandle,
						 ecs::ComponentGroup& components) const override {
		const wz::Event& _e = *static_cast<const wz::Event*>(eventHandle);

        switch (_e.GetEventType()) {
            case wz::EventType::app_render: SubmitGUI(); break;
            default: break;
        }
	}
};

class CameraSystem
    : public ecs::System {
public:
    CameraSystem() {
		AddComponentType<ViewComponent>();

        Subscribe((int32)wz::EventType::app_init);
        Subscribe((int32)wz::EventType::app_frame_begin);
        Subscribe((int32)wz::EventType::app_render);
        Subscribe((int32)wz::EventType::app_frame_end);
        Subscribe((int32)wz::EventType::window_resize);
	}

    void Init() const {
        WZ_CORE_TRACE("Initializing renderer from camera system");
        wz::RenderCommand::SetClearColor(.1f, .2f, .5f, 1.f);
        wz::RenderCommand::SetCullMode(wz::WZ_CULL_BACK);
        wz::RenderCommand::ToggleDepthTesting(true);
    }

    void BeginRenderer(ViewComponent& view) const {
        WZ_CORE_TRACE("Beginning renderer from camera system");
        wz::RenderCommand::Clear();
        wz::Renderer::Begin(view.ToMat4());
    }

    void OnRender(ViewComponent& view) const {
        ImGui::Begin("Camera view");

        ImGui::Text("Transform");
        ImGui::DragFloat3("Position##cam", glm::value_ptr(view.position), .1f);
        ImGui::DragFloat3("Rotation##cam", glm::value_ptr(view.rotation), .05f);

        ImGui::Spacing();

        ImGui::Text("Projection");
        ImGui::DragFloat("FOV##cam", &view.fov, .05f);
        static char _aspectPreview[1024] = "16:9";
#define ASPECT_RATIO_OPTION(x, y) \
{ \
    string _label = (string(#x) + ":" + string(#y)); \
    if (ImGui::MenuItem(_label.c_str())) { \
        view.aspectRatio = (float)x / (float)y; \
        strcpy(_aspectPreview, _label.c_str()); \
        view.dynamicAspectRatio = false;\
    } \
} \

        if (ImGui::BeginCombo("Aspect ratio", _aspectPreview)) {

            ASPECT_RATIO_OPTION(16, 9);

            ASPECT_RATIO_OPTION(16, 10);

            ASPECT_RATIO_OPTION(4, 3);

            ASPECT_RATIO_OPTION(9, 16);

            ASPECT_RATIO_OPTION(10, 16);

            ASPECT_RATIO_OPTION(3, 4);

            if (ImGui::MenuItem("Dynamic")) {
                view.dynamicAspectRatio = true;
                strcpy(_aspectPreview, "Dynamic");
            }

            ImGui::EndCombo();
        }

        ImGui::DragFloat("Near clipping##cam", &view.nearClip, .05f);
        ImGui::DragFloat("Far clipping##cam", &view.farClip, .05f);

        ImGui::End();
    }

    void EndRenderer(ViewComponent& view) const {
        WZ_CORE_TRACE("Ending Renderer from camera system");
        wz::Renderer::End();
    }

    void OnWindowResize(const wz::WindowResizeEvent& e, ViewComponent& view) const {
        wz::RenderCommand::SetViewport(0, 0, e.GetX(), e.GetY());
        if (view.dynamicAspectRatio) {
            view.aspectRatio = (float)e.GetX() / (float)e.GetY();
        }
    }

	virtual void OnEvent(const void* eventHandle,
						 ecs::ComponentGroup& components) const override {
		const wz::Event& _e = *static_cast<const wz::Event*>(eventHandle);

        ViewComponent& _view = *components.Get<ViewComponent>();

        switch (_e.GetEventType()) {
            case wz::EventType::app_init: Init(); break;
            case wz::EventType::app_frame_begin: BeginRenderer(_view); break;
            case wz::EventType::app_render: OnRender(_view); break;
            case wz::EventType::app_frame_end: EndRenderer(_view); break;
            case wz::EventType::window_resize:
            {
                const wz::WindowResizeEvent& _wE = *static_cast<const wz::WindowResizeEvent*>(eventHandle);
                OnWindowResize(_wE, _view);
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

	void Submit(RenderComponent& renderComponent,
				TransformComponent& transformComponent) const {
        WZ_CORE_TRACE("Submitting from render system");
		wz::Renderer::Submit(renderComponent.va, renderComponent.material,
						 transformComponent.ToMat4());

        auto& _material = renderComponent.material;

        string _wndLbl = "Entity Rendering##" + std::to_string(renderComponent.renderId);
        ImGui::Begin(_wndLbl.c_str());

        string _idStr = "Render ID: '" + std::to_string(renderComponent.renderId) + "'";
        ImGui::Text(_idStr.c_str());
        ImGui::Spacing();
        ImGui::Text("Material");

        auto& _shaderPreview = renderComponent.shaderPreview;
        auto& _texturePreview = renderComponent.texturePreview;
        auto& _diffuseModePreview = renderComponent.diffuseModePreview;

        _shaderPreview = wz::ResourceManagement::AliasOf(_material.shader);

        _texturePreview = _material.diffuseTexture ?
                    wz::ResourceManagement::AliasOf(_material.diffuseTexture)
                    : "";

        if (ImGui::BeginCombo("Shader", _shaderPreview.c_str())) {

            for (const auto& _alias : wz::ResourceManagement::GetAliases()) {
                if (wz::ResourceManagement::Get<wz::Shader>(_alias) != nullptr &&
                    ImGui::MenuItem(_alias.c_str())) {
                    _material.shader = wz::ResourceManagement::Get<wz::Shader>(_alias);
                    _shaderPreview = _alias;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Diffuse mode", _diffuseModePreview.c_str())) {
            if (ImGui::MenuItem("Texture##mode")) {
                if (!_material.diffuseTexture) {
                    for (const auto& _alias : wz::ResourceManagement::GetAliases()) {
                        if (wz::ResourceManagement::Get<wz::Texture>(_alias) != nullptr) {
                            _material.diffuseTexture =
                                wz::ResourceManagement::Get<wz::Texture>(_alias);
                            _diffuseModePreview = "Texture";
                            break;
                        }
                    }
                } else {
                    _diffuseModePreview = "Texture";
                }
            }
            if (ImGui::MenuItem("Color##mode")) {
                _material.diffuseTexture = nullptr;
                _diffuseModePreview = "Color";
            }
            ImGui::EndCombo();
        }

        if (_diffuseModePreview == "Texture") {
            if (ImGui::BeginCombo("Diffuse", _texturePreview.c_str())) {
                for (auto _alias : wz::ResourceManagement::GetAliases()) {

                    if (wz::ResourceManagement::Get<wz::Texture>(_alias) != nullptr &&
                        ImGui::MenuItem(_alias.c_str())) {
                        _material.diffuseTexture = wz::ResourceManagement::Get<wz::Texture>(_alias);
                        _texturePreview = _alias;
                    }
                }
                ImGui::EndCombo();
            }
        } else if (_diffuseModePreview == "Color") {
            ImGui::ColorEdit4("Diffuse", _material.diffuseColor.rgba);
        }

        ImGui::End();
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
		AddComponentType<PlayerTestTriangleComponent>(FLAG_OPTIONAL);

		Subscribe((int32)wz::AppUpdateEvent::GetStaticType());
		Subscribe((int32)wz::AppRenderEvent::GetStaticType());
	}

	void OnPlayerUpdate(TransformComponent& transform, float delta) const {
        WZ_CORE_TRACE("Player update");
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

        ImGui::Text("Transform");
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

    void CreateCamera() {
        ViewComponent _viewComp;
        _viewComp.position = vec3(0, 0, 10);
        _viewComp.rotation = vec3(0, wz::to_radians(180), 0);
        _viewComp.fov = 65;
        _viewComp.aspectRatio = 16.f / 9.f;
        _viewComp.nearClip = .1f;
        _viewComp.farClip = 10000.f;
        ecs::IComponent* _comps[] = {
            &_viewComp
        };
        ecs::StaticCId _ids[] = {
            _viewComp.staticId
        };
        m_ecs.CreateEntity(_comps, _ids, 1);
    }

    void CreateResourceGUI() {
        ResourceGUIComponent _guiComp;
        ecs::IComponent* _comps[] = {
            &_guiComp
        };
        ecs::StaticCId _ids[] = {
            ResourceGUIComponent::staticId
        };
        m_ecs.CreateEntity(_comps, _ids, 1);
    }

	void CreateTriangle(const wz::VertexArrayPtr& va, const wz::Material& mat, vec3 pos, vec3 rot, bool isPlayer = false) {
		RenderComponent _renderComp {va, mat};
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

    ~Sandbox() {

    }

	virtual
    void Init() override {
        //wz::Log::SetCoreLogLevel(LOG_LEVEL_TRACE);
		wz::ResourceManagement::SetResourcePath(string(BASE_DIR) + "Resources/");

        ulib::Bitset _pyramidFlags;
        ulib::Bitset _prismFlags;

		wz::ResourceManagement::Import<wz::Shader> (
											"basic.shader",
											"basicShader"
										);
        wz::ResourceManagement::Import<wz::Texture> (
											"pyramid.jpeg",
											"pyramidTexture001",
                                            _pyramidFlags
										);
        wz::ResourceManagement::Import<wz::Texture> (
											"prism.jpeg",
											"prismTexture001",
                                            _prismFlags
										);
        wz::ResourceManagement::Import<wz::Script> (
											"testScript.lua",
											"testScript"
										);
        wz::Texture *_texture = wz::ResourceManagement::Get<wz::Texture>("pyramidTexture001");

		wz::Shader *_shader = wz::ResourceManagement::Get<wz::Shader>("basicShader");
		wz::VertexArrayPtr _vao(wz::VertexArray::Create());
        float _vertices[] = {
            // near bot left
			-0.5f, -0.5f, -0.5f, 		0.0f, 1.0f,
            // near bot right
			 0.5f, -0.5f, -0.5f, 		1.0f, 1.0f,
            // top
			 0.0f,  0.5f, 0.0f, 		0.5f, 0.5f,
             // far bot left
 			-0.5f, -0.5f, 0.5f, 		0.0f, 0.0f,
             // far bot right
 			 0.5f, -0.5f, 0.5f, 		1.0f, 0.0f,
            };
		wz::VertexBufferPtr _vbo(wz::VertexBuffer::Create(_vertices, sizeof(_vertices)));

		wz::BufferLayout _layout = {
			{ wz::ShaderDataType::FLOAT3, "vertexPosition" },
			{ wz::ShaderDataType::FLOAT2, "vertexUv" }
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

        CreateCamera();
        CreateResourceGUI();
		CreateTriangle(_vao, { _shader }, vec3(-1.5f, 0.f, 0.f), vec3(0, 0, 0));
		CreateTriangle(_vao, { _shader }, vec3(1.5f, 0.f, 0.f), vec3(0, 0, 0));
        wz::Material _playerMaterial(_shader);
        _playerMaterial.diffuseTexture = _texture;
		//CreateTriangle(_vao, _playerMaterial, vec3(0.f, 0.f, 0.f), vec3(0, 0, 0), true);

        m_clientSystems.AddSystem<CameraSystem>();
        m_clientSystems.AddSystem<TestTriangleSystem>();
        m_clientSystems.AddSystem<RenderSystem>();

        m_engineSystems.AddSystem<ResourceGUISystem>();
	}

    virtual
    void Shutdown() override {

    }

};

Wizzy::Application* CreateApplication() {
    return new Sandbox();
}
