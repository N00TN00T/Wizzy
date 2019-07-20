#pragma once

struct ModelComponent
 	: public ecs::Component<ModelComponent> {
    char *handle;

    virtual void ShowGUI() override {

        auto& _model = wz::ResourceManagement::Get<wz::Model>(handle);
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
    }
};

struct EnvironmentComponent
    : public ecs::Component<EnvironmentComponent> {
    wz::RenderEnvironment value;

    virtual void ShowGUI() override {
        ImGui::Text("Environment");

        ImGui::ColorEdit4("Ambient", value.ambient.rgba);

        ImGui::ColorEdit4("Clear color", value.clearColor.rgba);
    }
};

struct ViewComponent
    : public ecs::Component<ViewComponent> {

    // projection
    float fov;
    float nearClip;
    float farClip;
    vec2 resolution;
    bool dynamicResolution = false;


    // control settings
    float panSpeed = 10.f;
    float rotationSpeed = 1.f;
    float orbitSpeed = 1.f;

    wz::RenderTargetPtr renderTarget;
    vec2 previousResolution;

    mat4 ToMat4() {
        mat4 _projection = glm::identity<mat4>();
        double _aspect = resolution.x / resolution.y;
        _projection = glm::perspectiveFov<double>(wz::to_radians(fov),
                                                 1.0 * _aspect,
                                                 1.0,
                                                 nearClip,
                                                 farClip);

        return _projection;
    }

    virtual void ShowGUI() override {
        ImGui::Text("Projection");
        ImGui::DragFloat("FOV##cam", &fov, .05f);
        ImGui::DragFloat("Near clipping##cam", &nearClip, .05f);
        ImGui::DragFloat("Far clipping##cam", &farClip, .05f);

        ImGui::Checkbox("Dynamic resolution", &dynamicResolution);
        if (!dynamicResolution) {
            ImGui::DragFloat2("Resolution##cam", glm::value_ptr(resolution));
        }
    }
};
