#pragma once

#include "Wizzy/Resources/Shader.h"

namespace Wizzy {

	typedef std::shared_ptr<Shader> ShaderPtr;

	class Material {
	public:
		Material();
		Material(ShaderPtr shader);
		~Material();

		inline ShaderPtr GetShader() const { return m_shader; }
		inline void SetShader(ShaderPtr shader) { m_shader = shader; }

		inline Color GetAlbedo() const { return m_albedo; }
		inline void SetAlbedo(const Color& albedo) { m_albedo = albedo; }

		inline Color GetDiffuse() const { return m_diffuse; }
		inline void SetDiffuse(Color color) { m_diffuse = color; }

		inline Color GetSpecular() const { return m_specular; }
		inline void SetSpecular(Color color) { m_specular = color; }

		inline float GetShininess() const { return m_shininess; }
		inline void SetShininess(float intensity) { m_shininess = intensity; }

		inline u32 GetId() const { return m_Id; }

	private:
		u32 m_Id = ++s_idCounter;
		ShaderPtr m_shader;
		Color m_albedo;
		Color m_diffuse;
		Color m_specular;
		float m_shininess;

	public:
		

	private:
		static u32 s_idCounter;
	};
}
