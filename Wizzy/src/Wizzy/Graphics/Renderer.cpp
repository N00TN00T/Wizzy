#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Graphics/Renderer.h"
#include "Wizzy/Graphics/GLErrorHandling.h"

namespace Wizzy {
	Renderer::Renderer() {
		WZ_CORE_TRACE("Constructed a renderer");
	}
	Renderer::~Renderer() {
		WZ_CORE_TRACE("Destructed a renderer");
	}
	void Renderer::Begin() {
	}
	void Renderer::SubmitRaw(const VertexArray& vao,
							 const IndexBuffer& ibo,
							 const Material& material,
							 const mat4& projection,
							 const mat4& view,
							 const mat4& model,
							 const vec3& viewPos) {
		m_commandQueue.push(RenderCommand(vao, ibo, material,
										  projection, view,
										  model, viewPos));
	}
	void Renderer::End(const Light& light) {

		while (m_commandQueue.size() > 0) {
			auto& _renderCommand = m_commandQueue.front();

			auto& _material = _renderCommand.material;

			_renderCommand.vao.Bind();
			_renderCommand.ibo.Bind();

			auto _shader = (_material.GetShader());
			_shader->Bind();

			_shader->SetUniformMat4("projection", _renderCommand.projection);
			_shader->SetUniformMat4("view", _renderCommand.view);
			_shader->SetUniformMat4("model", _renderCommand.model);
			
			_shader->SetUniform3f("viewPos", _renderCommand.projection * 
											_renderCommand.view *							                   vec4(_renderCommand.viewPos, 1.0));

			_shader->SetUniform3f("light.position", _renderCommand.projection * 
													_renderCommand.view * 
													vec4(light.position, 1.0));
			_shader->SetUniform3f("light.ambient", light.ambient.ToVec4());
			_shader->SetUniform3f("light.diffuse", light.diffuse.ToVec4());
			_shader->SetUniform3f("light.specular", light.specular.ToVec4());
			_shader->SetUniform1f("light.intensity", light.intensity);
			_shader->SetUniform1f("light.range", light.range);

			_shader->SetUniform4f("material.albedo", _material.GetAlbedo().ToVec4());
			_shader->SetUniform3f("material.diffuse", _material.GetDiffuse().ToVec4());
			_shader->SetUniform3f("material.specular", _material.GetSpecular().ToVec4());
			_shader->SetUniform1f("material.shininess", _material.GetShininess());

			GL_CALL(glDrawElements(GL_TRIANGLES,
								   _renderCommand.ibo.GetCount(),
								   GL_UNSIGNED_INT,
								   0));

			m_commandQueue.pop();

			_shader->Unbind();

			_renderCommand.ibo.Unbind();
			_renderCommand.vao.Unbind();
		}
	}

}
