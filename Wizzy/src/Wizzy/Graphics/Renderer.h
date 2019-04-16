#pragma once

#include "Wizzy/Graphics/VertexArray.h"
#include "Wizzy/Graphics/IndexBuffer.h"
#include "Wizzy/Graphics/Material.h"

namespace Wizzy {

	struct RenderCommand {
		const VertexArray& vao;
		const IndexBuffer& ibo;
		const Material& material;
		const mat4& transformation;

		RenderCommand(const VertexArray& vao, 
					  const IndexBuffer& ibo,
					  const Material& material, 
					  const mat4& transformation)
		: vao(vao), ibo(ibo), material(material), transformation(transformation) {}
	};

	class Renderer {
	public:

		Renderer();
		~Renderer();

		void Begin();
		void SubmitRaw(const VertexArray& vao, 
					   const IndexBuffer& ibo, 
					   const Material& material,
					   const mat4& transformation);
		void End();

	private:
		std::queue<RenderCommand> m_commandQueue;
	};
}