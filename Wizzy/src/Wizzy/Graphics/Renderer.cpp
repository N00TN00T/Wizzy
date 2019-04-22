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
							 const mat4& transformation) {
		m_commandQueue.push(RenderCommand(vao, ibo, material,
										  transformation ));
	}
	void Renderer::End() {

		while (m_commandQueue.size() > 0) {
			auto& _renderCommand = m_commandQueue.front();

			auto& _material = _renderCommand.material;

			WZ_CORE_DEBUG("DRAWING WITH ALBEDO {0}, {1}, {2}, {3}",
							_material.GetAlbedo().ToVec4().r,
							_material.GetAlbedo().ToVec4().g,
							_material.GetAlbedo().ToVec4().b,
							_material.GetAlbedo().ToVec4().a);

			_renderCommand.vao.Bind();
			_renderCommand.ibo.Bind();

			auto _shader = (_material.GetShader());
			_shader->Bind();

			_shader->SetUniformMat4("mvp", _renderCommand.transformation);
			_shader->SetUniform4f("albedo", _material.GetAlbedo().ToVec4());

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
