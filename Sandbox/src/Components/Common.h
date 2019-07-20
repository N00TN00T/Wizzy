#pragma once

inline string LightTypeToString(wz::LightType type) {
    switch (type) {
        case wz::LightType::DIRECTIONAL: return "Directional light";
        case wz::LightType::SPOT: return "Spot light";
        case wz::LightType::POINT: return "Point light";
        case wz::LightType::NONE: return "NONE";
    }
    return "";
}

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

	virtual void ShowGUI() override {
		ImGui::DragFloat3("Position", glm::value_ptr(position), .05f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), .01f);
        ImGui::DragFloat3("Scale", glm::value_ptr(scale), .01f);
    }

};

struct LightComponent
    : public ecs::Component<LightComponent> {
    wz::LightType type;
    wz::Color color;
    float range;
    float intensity;
    float cutOff = 12.5f;
    float smoothness = .2f;

	virtual void ShowGUI() override {
		#define LIGHT_TYPE_OPTION(l)\
        if (ImGui::Selectable(LightTypeToString(l).c_str())) { \
            type = l;\
        }

        string _typeStr = LightTypeToString(type);
        if (ImGui::BeginCombo("Light type", _typeStr.c_str())) {

            LIGHT_TYPE_OPTION(wz::LightType::DIRECTIONAL);

            LIGHT_TYPE_OPTION(wz::LightType::POINT);

            LIGHT_TYPE_OPTION(wz::LightType::SPOT);

            ImGui::EndCombo();
        }

        ImGui::ColorEdit4("Color", color.rgba);

        ImGui::DragFloat("Intensity", &intensity, .01f);

        if (type != wz::LightType::DIRECTIONAL) {
            ImGui::DragFloat("Range", &range, .1f);
        }

        if (type == wz::LightType::SPOT) {
            ImGui::DragFloat("Cutoff", &cutOff);
            ImGui::SliderFloat("Smoothness", &smoothness, .01f, 1.f);
        }
    }
};

struct EntityInfo
	: public ecs::Component<EntityInfo> {
	string *name;
	bool active = true;
	bool selected = false;
};
