#include "wzpch.h"

#include "Wizzy/Graphics/Material.h"

namespace Wizzy {

	u32 Material::s_idCounter(0);

	Material::Material()
		: m_albedo(Color::white), m_diffuse(Color::white), 
		m_shader(Shader::BasicShader()), m_specular(Color::white), m_shininess(32) {
		
	}

	Material::Material(ShaderPtr shader)
		: m_albedo(Color::white), m_diffuse(Color::white), 
		m_shader(shader), m_specular(Color::white), m_shininess(32) {
		
	}

	Material::~Material() {

	}
}
