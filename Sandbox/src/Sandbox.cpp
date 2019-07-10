#include "spch.h"
#include <Wizzy.h>

struct ModelComponent
 	: public ecs::Component<ModelComponent> {
    wz::ModelHandle handle;
};

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

struct ViewComponent
    : public ecs::Component<ViewComponent> {
    // view
    vec3 position;
    vec3 rotation;

    // projection
    float fov;
    float aspectRatio;
    float nearClip;
    float farClip;
    bool dynamicAspectRatio = false;

    // control settings
    float panSpeed = 10.f;
    float rotationSpeed = 1.f;
    float orbitSpeed = 1.f;

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

class RenderSystem
    : public ecs::System {
public:
    RenderSystem() {
        AddComponentType<TransformComponent>();
        AddComponentType<ModelComponent>(FLAG_OPTIONAL);

        Subscribe((int32)wz::EventType::app_render);
    }

    void SubmitModel(TransformComponent& transform, ModelComponent& modelComponent) const {
        WZ_CORE_TRACE("Submitting model from handle '{0}'", modelComponent.handle);
        auto& _model = *wz::ResourceManagement::Get<wz::Model>(modelComponent.handle);

        const auto& _meshes = _model.GetMeshes();
        for (const auto& _mesh : _meshes) {
            auto& _material = *wz::ResourceManagement::Get<wz::Material>(_mesh.GetMaterialHandle());
            wz::Renderer::Submit(_mesh.GetVAO(), _material, transform.ToMat4());
        }

        string _wndTitle = "Model##" + modelComponent.handle;
        ImGui::Begin(_wndTitle.c_str());

        ImGui::DragFloat3("Position", glm::value_ptr(transform.position), .05f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), .01f);
        ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), .01f);

        ImGui::End();
    }

    virtual void OnEvent(const void* eventHandle,
						 ecs::ComponentGroup& components) const override {
		const wz::Event& _e = *static_cast<const wz::Event*>(eventHandle);

        TransformComponent& _transform = *components.Get<TransformComponent>();

        switch (_e.GetEventType()) {
            case wz::EventType::app_render:
            {
                if (components.Has<ModelComponent>()) {
                    ModelComponent& _model = *components.Get<ModelComponent>();
                    SubmitModel(_transform, _model);
                }
                break;
            }
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
        Subscribe((int32)wz::EventType::app_update);
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

    void OnUpdate(ViewComponent& view, float delta) const {

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
            case wz::EventType::app_update:
            {
                auto& _uE = *static_cast<const wz::AppUpdateEvent*>(eventHandle);
                OnUpdate(_view, _uE.GetDeltaTime());
                break;
            }
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

class Sandbox
	: public Wizzy::Application {
public:

	Sandbox() {

	}

    ~Sandbox() {

    }

    void CreateCamera() {
        ViewComponent _viewComp;
        _viewComp.position = vec3(2.5f, -8, 14);
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
        m_clientEcs.CreateEntity(_comps, _ids, 1);
    }

    void CreateModel(wz::ModelHandle handle) {
        WZ_CORE_DEBUG("Creating model from handle '{0}'", handle);
        TransformComponent _transform;
        _transform.position = vec3(0.0, 0.0, 0.0);
        _transform.scale = vec3(1.0, 1.0, 1.0);
        _transform.rotation = vec3(0.0, 0.0, 0.0);

        ModelComponent _model;
        _model.handle = handle;

        ecs::IComponent* _comps[] = {
            &_transform, &_model
        };
        ecs::StaticCId _ids[] = {
            _transform.staticId, _model.staticId
        };

        m_clientEcs.CreateEntity(_comps, _ids, 2);
    }

	virtual
    void Init() override {
        wz::Log::SetCoreLogLevel(LOG_LEVEL_TRACE);

        wz::Flagset _scriptFlags;
        _scriptFlags.SetBit(wz::SCRIPT_LUA);

        string _projectDir = BASE_DIR;
        wz::ResourceManagement::Load<wz::Shader>(
            _projectDir + "TestResources/basic.shader",
            "DefaultShader"
        );
        wz::ResourceManagement::Load<wz::Shader>(
            _projectDir + "TestResources/phong_basic.shader",
            WZ_DEFAULT_SHADER_HANDLE
        );
        wz::ResourceManagement::Load<wz::Model>(
            _projectDir + "TestResources/nanosuit/nano.fbx",
            "Nanosuit"
        );
        wz::ResourceManagement::Load<wz::Model>(
            _projectDir + "TestResources/chair.fbx",
            "Chair"
        );
        wz::ResourceManagement::Load<wz::Model>(
            _projectDir + "TestResources/sword.fbx",
            "Sword"
        );
        wz::ResourceManagement::Load<wz::Script>(
            _projectDir + "TestResources/testScript.lua",
            "TestScript",
            _scriptFlags
        );
        wz::ResourceManagement::Load<wz::Texture>(
            _projectDir + "TestResources/prism.jpeg",
            "PrismTexture001"
        );
        wz::ResourceManagement::Load<wz::Material>(
            _projectDir + "TestResources/testMaterial.mat",
            "TestMaterial"
        );

        CreateCamera();
        CreateModel("Chair");
        CreateModel("Nanosuit");

        m_clientSystems.AddSystem<CameraSystem>();
        m_clientSystems.AddSystem<RenderSystem>();
	}

    virtual
    void Shutdown() override {

    }

};

Wizzy::Application* CreateApplication() {
    return new Sandbox();
}
