#include <wzpch.h>

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/Renderer.h"
#include "Wizzy/Resources/Shader.h"
#include "Wizzy/Resources/Texture2D.h"

namespace Wizzy {
	Renderer::Renderer() {
	}
	void Renderer::Draw(Texture2D * tex, Shader * shader, const mat4& model) {
		tex->Bind(0x84C0);
		shader->Bind();
		shader->SetUniform1i("ourTexture", 0);
		shader->SetUniformMat4("ourMatrix", model);
		
		GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

		tex->Unbind();
		shader->Unbind();
	}
}