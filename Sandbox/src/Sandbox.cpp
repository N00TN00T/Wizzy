#include "spch.h"
#include <Wizzy.h>

struct ModelComponent
 	: public ecs::Component<ModelComponent> {
    char *handle;
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

struct EnvironmentComponent
    : public ecs::Component<EnvironmentComponent> {
    wz::RenderEnvironment value;
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

struct LightComponent
    : public ecs::Component<LightComponent> {
    wz::LightType type;
    wz::Color color;
    float range;
    float intensity;
};

class RenderSystem
    : public ecs::System {
public:
    RenderSystem() {
        AddComponentType<TransformComponent>();
        AddComponentType<ModelComponent>(FLAG_OPTIONAL);
        AddComponentType<LightComponent>(FLAG_OPTIONAL);

        Subscribe((int32)wz::EventType::app_render);
    }

    string LightTypeToString(wz::LightType type) const {
        switch (type) {
            case wz::LightType::DIRECTIONAL: return "Directional light";
            case wz::LightType::SPOT: return "Spot light";
            case wz::LightType::POINT: return "Point light";
            case wz::LightType::NONE: return "NONE";
        }
        return "";
    }

    void SubmitLight(TransformComponent& transform, LightComponent& light) const {

        #define LIGHT_TYPE_OPTION(l)\
        if (ImGui::Selectable(LightTypeToString(l).c_str())) { \
            light.type = l;\
        }

        string _typeStr = LightTypeToString(light.type);
        if (ImGui::BeginCombo("Light type", _typeStr.c_str())) {

            LIGHT_TYPE_OPTION(wz::LightType::DIRECTIONAL);

            LIGHT_TYPE_OPTION(wz::LightType::POINT);

            LIGHT_TYPE_OPTION(wz::LightType::SPOT);

            ImGui::EndCombo();
        }

        ImGui::ColorEdit4("Color", light.color.rgba);

        ImGui::DragFloat("Intensity", &light.intensity, .01f);

        ImGui::DragFloat("Range", &light.range, .1f);

        wz::Renderer::SubmitLight(light.type, transform.position, transform.rotation, light.color, light.range, light.intensity);
    }

    void SubmitModel(TransformComponent& transform, ModelComponent& modelComponent) const {
        WZ_CORE_TRACE("Submitting model from handle '{0}'", modelComponent.handle);
        auto& _model = wz::ResourceManagement::Get<wz::Model>(modelComponent.handle);

        auto& _meshes = _model.GetMeshes();

        if (ImGui::TreeNode("Meshes")) {
            WZ_CORE_TRACE("Iterating meshes in model to submit to gui");
            for (u32 i = 0; i < _meshes.size(); i++) {
                auto& _mesh = _meshes.at(i);
                if (ImGui::TreeNode(_mesh.GetName().c_str())) {
                    auto& _mat = wz::ResourceManagement::Get<wz::Material>(_mesh.GetMaterialHandle());

                    ImGui::TextUnformatted(_mesh.GetMaterialHandle().c_str());

                    ImGui::Spacing();

                    ImGui::ColorEdit4("Albedo", _mat.albedo.rgba);

                    bool _validDiffuse = wz::ResourceManagement::Is<wz::Texture>(_mat.diffuseMapHandle);
                    if (ImGui::BeginCombo("Diffuse map", _validDiffuse ? _mat.diffuseMapHandle.c_str() :
                                                                         "None")) {
                        if (ImGui::Selectable("None")) {
                            _mat.diffuseMapHandle = WZ_NULL_RESOURCE_HANDLE;
                            _validDiffuse = false;
                        }
                        ImGui::Separator();
                        for (const auto& _handle : wz::ResourceManagement::GetHandles()) {
                            if (wz::ResourceManagement::Is<wz::Texture>(_handle)) {
                                wz::Texture& _texture = wz::ResourceManagement::Get<wz::Texture>(_handle);
                                double _aspect = (double)_texture.GetWidth() / (double)_texture.GetHeight();
                                int32 _height = 32.0 / _aspect;
                                ImGui::TextUnformatted(_handle.c_str());
                                if (ImGui::ImageButton(reinterpret_cast<void*>(_texture.GetId()), ImVec2(32, _height))) {
                                    _mat.diffuseMapHandle = _handle;
                                }
                            }
                        }
                        ImGui::EndCombo();
                    }
                    WZ_CORE_TRACE("Deciding if diffuse color option should show");

                    if (_validDiffuse) {
                        wz::Texture& _diffuse = wz::ResourceManagement::Get<wz::Texture>(_mat.diffuseMapHandle);
                        ImGui::Image(reinterpret_cast<void*>(_diffuse.GetId()), ImVec2(100, 100));
                    } else {
                        ImGui::ColorEdit4("Diffuse color", _mat.diffuseColor.rgba);
                    }


                    bool _validSpecular = wz::ResourceManagement::Is<wz::Texture>(_mat.specularMapHandle);
                    if (ImGui::BeginCombo("Specular map", _validSpecular ? _mat.specularMapHandle.c_str() :
                                                                         "None")) {
                        if (ImGui::Selectable("None")) {
                            _mat.specularMapHandle = WZ_NULL_RESOURCE_HANDLE;
                            _validSpecular = false;
                        }
                        ImGui::Separator();
                        for (const auto& _handle : wz::ResourceManagement::GetHandles()) {
                            if (wz::ResourceManagement::Is<wz::Texture>(_handle)) {
                                wz::Texture& _texture = wz::ResourceManagement::Get<wz::Texture>(_handle);
                                double _aspect = (double)_texture.GetWidth() / (double)_texture.GetHeight();
                                int32 _height = 32.0 / _aspect;
                                ImGui::TextUnformatted(_handle.c_str());
                                if (ImGui::ImageButton(reinterpret_cast<void*>(_texture.GetId()), ImVec2(32, _height))) {
                                    _mat.specularMapHandle = _handle;
                                }
                            }
                        }
                        ImGui::EndCombo();
                    }
                    WZ_CORE_TRACE("Deciding if specular color option should show");

                    if (_validSpecular) {
                        wz::Texture& _specular = wz::ResourceManagement::Get<wz::Texture>(_mat.specularMapHandle);
                        ImGui::Image(reinterpret_cast<void*>(_specular.GetId()), ImVec2(100, 100));
                        ImGui::SliderFloat("Metallic", &_mat.specularColor.a, .01f, 1.f);
                    } else {
                        ImGui::ColorEdit4("Specular color", _mat.specularColor.rgba);
                    }

                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        for (wz::Mesh& _mesh : _meshes) {
            auto& _material = wz::ResourceManagement::Get<wz::Material>(_mesh.GetMaterialHandle());
            wz::Renderer::Submit(_mesh.GetVAO(), _material, transform.ToMat4());
        }
    }

    virtual void OnEvent(const void* eventHandle,
						 ecs::ComponentGroup& components) const override {
		const wz::Event& _e = *static_cast<const wz::Event*>(eventHandle);

        TransformComponent& _transform = *components.Get<TransformComponent>();

        switch (_e.GetEventType()) {
            case wz::EventType::app_render:
            {
                string _wndTitle = "Entity inspector##" + std::to_string(reinterpret_cast<uintptr_t>(_transform.entity));
                ImGui::Begin(_wndTitle.c_str());
                ImGui::DragFloat3("Position", glm::value_ptr(_transform.position), .05f);
                ImGui::DragFloat3("Rotation", glm::value_ptr(_transform.rotation), .01f);
                ImGui::DragFloat3("Scale", glm::value_ptr(_transform.scale), .01f);
                if (components.Has<ModelComponent>()) {
                    ModelComponent& _model = *components.Get<ModelComponent>();
                    SubmitModel(_transform, _model);
                }
                if (components.Has<LightComponent>()) {
                    LightComponent& _light = *components.Get<LightComponent>();
                    SubmitLight(_transform, _light);
                }
                ImGui::End();
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
        AddComponentType<EnvironmentComponent>();

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

    void BeginRenderer(ViewComponent& view, EnvironmentComponent& environment) const {
        WZ_CORE_TRACE("Beginning renderer from camera system");
        wz::RenderCommand::Clear();
        wz::Renderer::Begin(view.ToMat4(), view.position, environment.value);
    }

    void OnUpdate(ViewComponent& view, float delta) const {

    }

    void OnRender(ViewComponent& view, EnvironmentComponent& environment) const {
        ImGui::Begin("Camera");

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

        ImGui::Spacing();

        ImGui::Text("Environment");

        ImGui::ColorEdit4("Ambient", environment.value.ambient.rgba);

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
        EnvironmentComponent& _environment = *components.Get<EnvironmentComponent>();

        switch (_e.GetEventType()) {
            case wz::EventType::app_init: Init(); break;
            case wz::EventType::app_frame_begin: BeginRenderer(_view, _environment); break;
            case wz::EventType::app_update:
            {
                auto& _uE = *static_cast<const wz::AppUpdateEvent*>(eventHandle);
                OnUpdate(_view, _uE.GetDeltaTime());
                break;
            }
            case wz::EventType::app_render: OnRender(_view, _environment); break;
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
        _viewComp.position = vec3(2.5f, -8, 20);
        _viewComp.rotation = vec3(0, wz::to_radians(180), 0);
        _viewComp.fov = 65;
        _viewComp.aspectRatio = 16.f / 9.f;
        _viewComp.nearClip = .1f;
        _viewComp.farClip = 10000.f;

        EnvironmentComponent _environment;

        ecs::IComponent* _comps[] = {
            &_viewComp, &_environment
        };
        ecs::StaticCId _ids[] = {
            _viewComp.staticId, _environment.staticId
        };
        m_clientEcs.CreateEntity(_comps, _ids, 2);
    }

    void CreateModel(string handle, vec3 position = vec3(0, 0, 0), vec3 rotation = vec3(0, 0, 0), vec3 scale = vec3(1, 1, 1)) {
        TransformComponent _transform;
        _transform.position = position;
        _transform.scale = scale;
        _transform.rotation = rotation;

        ModelComponent _model;
        _model.handle = new char[handle.size()];
        strcpy(_model.handle, handle.c_str());


        ecs::IComponent* _comps[] = {
            &_transform, &_model
        };
        ecs::StaticCId _ids[] = {
            _transform.staticId, _model.staticId
        };

        m_clientEcs.CreateEntity(_comps, _ids, 2);
    }

    void CreateLamp(string modelHandle, wz::LightType type, vec3 position = vec3(0, 0, 0), vec3 rotation = vec3(0, 0, 0), vec3 scale = vec3(1, 1, 1)) {
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
        _light.intensity = 1.5f;
        _light.range = 25.f;

        ecs::IComponent* _comps[] = {
            &_transform, &_model, &_light
        };
        ecs::StaticCId _ids[] = {
            _transform.staticId, _model.staticId, _light.staticId
        };

        m_clientEcs.CreateEntity(_comps, _ids, 3);
    }

    void CreateSun() {
        TransformComponent _transform;
        _transform.rotation = vec3(0, 0, -1);

        LightComponent _light;
        _light.type = wz::LightType::DIRECTIONAL;
        _light.color = wz::Color(.8f, .85f, .85f, 1.f);
        _light.intensity = 2.f;

        ecs::IComponent* _comps[] = {
            &_transform, &_light
        };
        ecs::StaticCId _ids[] = {
            _transform.staticId, _light.staticId
        };

        m_clientEcs.CreateEntity(_comps, _ids, 2);
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
        CreateModel("Nanosuit", vec3(-10, 0, 0), vec3(0, wz::to_radians(180), 0));
        CreateModel("Nanosuit", vec3(10, 0, 0), vec3(0, wz::to_radians(180), 0));
        //CreateLamp("Lightbulb", wz::LightType::POINT, vec3(-5, 0, 0), vec3(0), vec3(.125f, .125f, .125f));
        CreateLamp("Lightbulb", wz::LightType::POINT, vec3(5, 0, 0), vec3(0), vec3(.125f, .125f, .125f));
        CreateSun();


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
