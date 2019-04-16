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

	private:
		ShaderPtr m_shader;
		Color m_albedo;

	private:
		static ShaderPtr DefaultShader();
	};
}