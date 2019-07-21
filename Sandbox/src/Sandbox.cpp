#include "spch.h"

#include "GUI.h"

class RenderSystem
    : public ecs::System {
public:
    RenderSystem() {
        AddComponentType<TransformComponent>();
        AddComponentType<ModelComponent>(FLAG_OPTIONAL);
        AddComponentType<LightComponent>(FLAG_OPTIONAL);

        Subscribe(wz::EventType::app_render);
    }

    void SubmitLight(TransformComponent& transform, LightComponent& light) const {

        wz::Renderer::SubmitLight(light.type, transform.position, transform.rotation, light.color, light.range, light.intensity, glm::radians(light.cutOff), light.smoothness);
    }

    void SubmitModel(TransformComponent& transform, ModelComponent& modelComponent) const {
        WZ_CORE_TRACE("Submitting model from handle '{0}'", modelComponent.handle);
        auto& _model = wz::ResourceManagement::Get<wz::Model>(modelComponent.handle);
        auto& _meshes = _model.GetMeshes();

        for (wz::Mesh& _mesh : _meshes) {
            auto& _material = wz::ResourceManagement::Get<wz::Material>(_mesh.GetMaterialHandle());
            wz::Renderer::Submit(_mesh.GetVAO(), _material, transform.ToMat4());
        }
    }

    virtual void OnEvent(const wz::Event& e,
						 ecs::ComponentGroup& components) const override {
        TransformComponent& _transform = *components.Get<TransformComponent>();

        switch (e.GetEventType()) {
            case wz::EventType::app_render:
            {
                if (components.Has<ModelComponent>()) {
                    ModelComponent& _model = *components.Get<ModelComponent>();
                    SubmitModel(_transform, _model);
                }
                if (components.Has<LightComponent>()) {
                    LightComponent& _light = *components.Get<LightComponent>();
                    SubmitLight(_transform, _light);
                }
                break;
            }
            default: WZ_CORE_ASSERT(false, "System doesn't handle subscribed event"); break;
        }
	}
};

class CameraSystem
    : public ecs::System {
public:
    CameraSystem() {
        AddComponentType<TransformComponent>();
		AddComponentType<ViewComponent>();
        AddComponentType<EnvironmentComponent>();

        Subscribe(wz::EventType::app_init);
        Subscribe(wz::EventType::app_render);
        Subscribe(wz::EventType::app_frame_end);
	}

    void Init(ViewComponent& view) const {
        WZ_CORE_TRACE("Initializing renderer from camera system");

        view.renderTarget = wz::RenderTargetPtr(wz::RenderTarget::Create(view.resolution.x, view.resolution.y));

        wz::Renderer::SetRenderTarget(view.renderTarget);

        wz::RenderCommand::SetCullMode(wz::WZ_CULL_BACK);
        wz::RenderCommand::ToggleDepthTesting(true);
        wz::RenderCommand::ToggleBlending(true);
    }

    void BeginRenderer(TransformComponent& transform, ViewComponent& view, EnvironmentComponent& environment) const {
        WZ_CORE_TRACE("Beginning renderer from camera system");

        if (view.renderTarget != wz::Renderer::GetRenderTarget() || view.resolution != view.previousResolution) {
            view.resolution.x = glm::max(view.resolution.x, 1.f);
            view.resolution.y = glm::max(view.resolution.y, 1.f);
            view.renderTarget->SetSize(view.resolution.x, view.resolution.y);
            wz::Renderer::SetRenderTarget(view.renderTarget);
            wz::RenderCommand::SetViewport(wz::Viewport(0, 0, view.resolution.x, view.resolution.y));
        }

        view.previousResolution = view.resolution;

        view.renderTarget->Bind();
        wz::RenderCommand::Clear();
        view.renderTarget->Unbind();

        mat4 _view = mat4(1.f);
        _view = glm::rotate(_view, transform.rotation.x, vec3(1, 0, 0));
		_view = glm::rotate(_view, transform.rotation.y, vec3(0, 1, 0));
		_view = glm::rotate(_view, transform.rotation.z, vec3(0, 0, 1));
        _view = glm::translate(_view, transform.position);

        mat4 _projection = view.ToMat4();

        wz::Renderer::Begin(_projection * glm::inverse(transform.ToMat4()), transform.position, environment.value);
    }

    void EndRenderer(ViewComponent& view) const {
        WZ_CORE_TRACE("Ending Renderer from camera system");
        wz::Renderer::End();
    }

	virtual void OnEvent(const wz::Event& e, ecs::ComponentGroup& components) const override {
        TransformComponent& _transform = *components.Get<TransformComponent>();
        ViewComponent& _view = *components.Get<ViewComponent>();
        EnvironmentComponent& _environment = *components.Get<EnvironmentComponent>();

        switch (e.GetEventType()) {
            case wz::EventType::app_init: Init(_view); break;
            case wz::EventType::app_render: BeginRenderer(_transform, _view, _environment); break;
            case wz::EventType::app_frame_end: EndRenderer(_view); break;
            default: WZ_CORE_ASSERT(false, "System doesn't handle subscribed event"); break;
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
        EntityInfo _infoComponent;
        _infoComponent.name = new string("Main Camera");
        _infoComponent.selected = true;

        TransformComponent _transformComp;
        _transformComp.position = vec3(0, 8, 20);
        _transformComp.rotation = vec3(0, 0, 0);
        _transformComp.scale = vec3(1, 1, 1);

        ViewComponent _viewComp;
        _viewComp.fov = 65;
        _viewComp.nearClip = .1f;
        _viewComp.farClip = 10000.f;
        _viewComp.resolution = vec2(1280, 720);

        EnvironmentComponent _environment;

        ecs::IComponent* _comps[] = {
            &_infoComponent, &_transformComp, &_viewComp, &_environment
        };
        ecs::StaticCId _ids[] = {
            _infoComponent.staticId, _transformComp.staticId, _viewComp.staticId, _environment.staticId
        };
        m_clientEcs.CreateEntity(_comps, _ids, 4);
    }

    void CreateModel(string handle, vec3 position = vec3(0, 0, 0), vec3 rotation = vec3(0, 0, 0), vec3 scale = vec3(1, 1, 1)) {
        EntityInfo _infoComponent;
        _infoComponent.name = new string(handle);
        _infoComponent.selected = true;

        TransformComponent _transform;
        _transform.position = position;
        _transform.scale = scale;
        _transform.rotation = rotation;

        ModelComponent _model;
        _model.handle = new char[handle.size()];
        strcpy(_model.handle, handle.c_str());


        ecs::IComponent* _comps[] = {
            &_infoComponent, &_transform, &_model
        };
        ecs::StaticCId _ids[] = {
            _infoComponent.staticId, _transform.staticId, _model.staticId
        };

        m_clientEcs.CreateEntity(_comps, _ids, 3);
    }

    void CreateLamp(string modelHandle, wz::LightType type, vec3 position = vec3(0, 0, 0), vec3 rotation = vec3(0, 0, 0), vec3 scale = vec3(1, 1, 1)) {
        EntityInfo _infoComponent;
        _infoComponent.name = new string(modelHandle + "Light");
        _infoComponent.selected = true;

        TransformComponent _transform;
        _transform.position = position;
        _transform.scale = scale;
        _transform.rotation = rotation;

        ModelComponent _model;
        _model.handle = new char[modelHandle.size()];
        strcpy(_model.handle, modelHandle.c_str());

        LightComponent _light;
        _light.type = type;
        _light.color = wz::Color(.8f, .85f, .85f, 1.f);
        _light.intensity = 1.f;
        _light.range = 25.f;

        ecs::IComponent* _comps[] = {
            &_infoComponent, &_transform, &_model, &_light
        };
        ecs::StaticCId _ids[] = {
            _infoComponent.staticId, _transform.staticId, _model.staticId, _light.staticId
        };

        m_clientEcs.CreateEntity(_comps, _ids, 4);
    }

    void CreateSun() {
        EntityInfo _infoComponent;
        _infoComponent.name = new string("Sun");
        _infoComponent.selected = true;

        TransformComponent _transform;
        _transform.rotation = vec3(0, 0, -1);

        LightComponent _light;
        _light.type = wz::LightType::DIRECTIONAL;
        _light.color = wz::Color(.8f, .85f, .85f, 1.f);
        _light.intensity = 1.f;

        ecs::IComponent* _comps[] = {
            &_infoComponent, &_transform, &_light
        };
        ecs::StaticCId _ids[] = {
            _infoComponent.staticId, _transform.staticId, _light.staticId
        };

        m_clientEcs.CreateEntity(_comps, _ids, 3);
    }

	virtual
    void Init() override {
        wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);

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
            _projectDir + "TestResources/nanosuit/nanosuit.blend",
            "Nanosuit"
        );
        wz::ResourceManagement::Load<wz::Model>(
            _projectDir + "TestResources/lightbulb/Lightbulb_General_Poly_OBJ.obj",
            "Lightbulb"
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


        for (auto& _mesh : wz::ResourceManagement::Get<wz::Model>("Lightbulb").GetMeshes()) {
            auto& _mat = wz::ResourceManagement::Get<wz::Material>(_mesh.GetMaterialHandle());
            _mat.diffuseMapHandle = wz::Texture::WhiteTexture();
        }


        CreateCamera();
        CreateModel("Nanosuit", vec3(0, 0, 0), vec3(0, 0, 0));
        //CreateLamp("Lightbulb", wz::LightType::POINT, vec3(-5, 0, 0), vec3(0), vec3(.125f, .125f, .125f));
        CreateLamp("Lightbulb", wz::LightType::POINT, vec3(5, 0, 0), vec3(0), vec3(.125f, .125f, .125f));
        CreateSun();

        m_clientSystems.AddSystem<CameraSystem>();
        m_clientSystems.AddSystem<RenderSystem>();
        m_clientSystems.AddSystem<GUISystem>();
	}

    virtual
    void Shutdown() override {

    }

};

Wizzy::Application* CreateApplication() {
    return new Sandbox();
}
