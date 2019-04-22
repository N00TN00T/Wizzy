#include "wzpch.h"

#include "Wizzy/Graphics/Material.h"

namespace Wizzy {

	Material::Material()
		: m_albedo(Color::white) {
		m_shader = DefaultShader();
	}

	Material::Material(ShaderPtr shader)
		: m_albedo(Color::white) {
		m_shader = shader;
	}

	Material::~Material() {

	}

	const Color Color::white =		Color(1.f, 1.f, 1.f, 1.f);
	const Color Color::red =		Color(.8f, .1f, .1f, 1.f);
	const Color Color::green =		Color(.1f, 1.f, .1f, 1.f);
	const Color Color::blue =		Color(.1f, .1f, 1.f, 1.f);
	const Color Color::cyan =		Color(.1f, 1.f, 1.f, 1.f);
	const Color Color::black =		Color(.0f, .0f, .0f, 1.f);
	const Color Color::darkGray =	Color(.25f, .25f, .25f, 1.f);
	const Color Color::gray =		Color(.5f, .5f, .5f, 1.f);
	const Color Color::lightGray =	Color(.75f, .75f, .75f, 1.f);
	const Color Color::pink =		Color(1.f, .25f, .35f, 1.f);
	const Color Color::magenta =	Color(1.f, .1f, .8f, 1.f);

	ShaderPtr Material::DefaultShader() {
		static ShaderPtr s_defaultShader =
			std::make_shared<Shader>(Shader(ShaderProgramSource{
R"(

#version 410 core

layout (location = 0) in vec4 vertexPosition;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec4 vertexNormal;
layout (location = 3) in vec2 vertexUv;

uniform mat4 mvp = mat4(1.0);

out DATA {
	vec4 color;
	vec2 uv;
} pass;

void main() {
	pass.color = vertexColor;
	pass.uv = vertexUv;
	vec4 worldPosition = mvp * vertexPosition;
	gl_Position = worldPosition;
}

)",
R"(

#version 410 core

out vec4 finalColor;

uniform vec4 albedo = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D tex;

in DATA {
	vec4 color;
	vec2 uv;
} pass;

void main() {
	finalColor = texture(tex, pass.uv) * pass.color * albedo;
}

)" }));
		return s_defaultShader;
	}
}
