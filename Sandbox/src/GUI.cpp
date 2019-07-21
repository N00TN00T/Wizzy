#include "spch.h"

#include "GUI.h"

bool GUISystem::_dock = false;

GUISystem::GUISystem() {
    AddComponentType<EntityInfo>();
    AddComponentType<TransformComponent>(FLAG_OPTIONAL);
    AddComponentType<ViewComponent>(FLAG_OPTIONAL);
    AddComponentType<ModelComponent>(FLAG_OPTIONAL);
    AddComponentType<LightComponent>(FLAG_OPTIONAL);
    AddComponentType<EnvironmentComponent>(FLAG_OPTIONAL);

    Subscribe(wz::EventType::app_render);
    Subscribe(wz::EventType::app_frame_end);
}

void GUISystem::DoGenericGUI(ecs::ComponentGroup& components) const {
    EntityInfo& _entity = *components.Get<EntityInfo>();
    string _wndTitle = *_entity.name + " inspector";
    ImGui::Begin(_wndTitle.c_str());
    for (auto& _pair : components.GetComponents()) {
        auto _id = _pair.first;
        auto _component = _pair.second;
        WZ_CORE_TRACE("Showing gui for component '{0}'",
            ecs::IComponent::StaticInfo(_id).name);
        _component->ShowGUI();
        ImGui::Spacing();
    }
    ImGui::End();
}

void GUISystem::RenderGameView(ViewComponent& view) const {
    ImGui::Begin("Game View", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImVec2 _windowSize = ImGui::GetWindowSize();
    if (view.dynamicResolution) {
        view.resolution = vec2(_windowSize.x, _windowSize.y);
    }// w / h = a; w = a * h; w / a = h
    float _aspect = view.resolution.x / view.resolution.y;
    float _resToWndRatio = _windowSize.x / view.resolution.x;
    ImVec2 _gameSize = ImVec2(ImVec2(view.resolution.x * _resToWndRatio, (view.resolution.x / _aspect) * _resToWndRatio));
    if (_gameSize.y > _windowSize.y) {
        _gameSize.y = _windowSize.y;
        _gameSize.x = _gameSize.y * _aspect;
    }
    ImGui::SetCursorPos(ImVec2(_windowSize.x / 2.f - _gameSize.x / 2.f, _windowSize.y / 2.f - _gameSize.y / 2.f));
    ImGui::Image(reinterpret_cast<void*>(view.renderTarget->GetTextureId()), _gameSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

void GUISystem::OnEvent(const wz::Event& e,
             ecs::ComponentGroup& components) const {
    switch (e.GetEventType()) {
        case wz::EventType::app_render:
        {
            WZ_CORE_TRACE("GUI on render for entity {0}",
                *(components.Get<EntityInfo>()->name));
            if (!_dock) {
                WZ_CORE_TRACE("Enabling docking");
                ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
                _dock = true;
            }

            if (components.Has<ViewComponent>()) {
                WZ_CORE_TRACE("Rendering game view");
                RenderGameView(*components.Get<ViewComponent>());
            }

            WZ_CORE_TRACE("Doing inspector UI");
            DoGenericGUI(components);

            break;
        }
        case wz::EventType::app_frame_end: _dock = false; break;
        default: WZ_CORE_ASSERT(false, "System doesn't handle subscribed event"); break;
    }
}
