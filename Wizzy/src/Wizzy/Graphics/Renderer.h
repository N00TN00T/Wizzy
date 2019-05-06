#pragma once

#include "Wizzy/Graphics/VertexArray.h"
#include "Wizzy/Graphics/IndexBuffer.h"
#include "Wizzy/Graphics/Material.h"

namespace Wizzy {

	struct RenderCommand {
		const VertexArray& vao;
		const IndexBuffer& ibo;
		const Material& material;
		const mat4& projection;
		const mat4& view;
		const mat4& model;
		const vec3& viewPos;

		RenderCommand(const VertexArray& vao, 
					  const IndexBuffer& ibo,
					  const Material& material, 
					  const mat4& projection,
					  const mat4& view,
					  const mat4& model,
					  const vec3& viewPos)
		: vao(vao), ibo(ibo), material(material), projection(projection), view(view), model(model), viewPos(viewPos) {}
	};

	struct Light {
		vec3 position = vec3(5, 5, -10);

		Color ambient = Color(.1f, .1f, .1f, 1.0f);
		Color diffuse = Color::white;
		Color specular = Color::white;
		float intensity = 1.0;
		float range = 30;
	};

	class Renderer {
	public:

		Renderer();
		~Renderer();

		void Begin();
		void SubmitRaw(const VertexArray& vao, 
					   const IndexBuffer& ibo, 
					   const Material& material,
					   const mat4& projection,
					   const mat4& view,
					   const mat4& model,
					   const vec3& viewPos);
		void End(const Light& light);

	private:
		std::queue<RenderCommand> m_commandQueue;
	};
}