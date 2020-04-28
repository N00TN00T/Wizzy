#include "wzpch.h"

#include "Wizzy/Renderer/Renderer2D.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Renderer/RenderCommand.h"

#include "Wizzy/Instrumentor.h"
#include "Wizzy/WizzyExceptions.h"
constexpr char fallback_shader_source[] = R"(

#shader vertex

#version 330 core

layout (location = 0) in vec2 v_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec4 v_color;
layout (location = 3) in float v_location;

out vec2 uv;
out vec4 color;
out float location;


uniform mat4 u_camTransform;

void main() {
    uv = v_uv;
	location = v_location;
	color = v_color;
    gl_Position = u_camTransform * vec4(v_position, 0.0, 1.0);
}


#shader fragment


#version 330 core


out vec4 fragColor;


in vec2 uv;
in vec4 color;
in float location;


uniform sampler2D u_textures[32];

void main() {
    fragColor = texture(u_textures[int(round(location))], uv) * color;

}

)";

namespace Wizzy
{
	Texture::Handle Renderer2D::s_hWhite1x1Texture(WZ_NULL_RESOURCE_HANDLE);
	Shader::Handle Renderer2D::s_hFallbackShader(WZ_NULL_RESOURCE_HANDLE);
	Renderer2D::Metrics Renderer2D::s_metrics;

	Renderer2D::Pipeline::Pipeline(const size_t& budget) 
		: budget(budget), numTextureSlots(MaxTextureSlot())
	{
		WZ_PROFILE_FUNCTION();
		pBufferHead = new VertexData[(budget) / sizeof(VertexData)];
		pBufferCurrent = pBufferHead;

		vbo = VertexBufferPtr(
			VertexBuffer::Create(NULL, budget, WZ_BUFFER_USAGE_DYNAMIC_DRAW)
		);

		BufferLayout layout = {
			{ ShaderDataType::FLOAT2, "v_position" },
			{ ShaderDataType::FLOAT2, "v_uv" },
			{ ShaderDataType::FLOAT4, "v_color" },
			{ ShaderDataType::FLOAT1, "v_location", true },
		};

		vbo->SetLayout(layout);

		u32* indices = new u32[maxIndices];

		int _offset = 0;
		for (int i = 0; i < maxIndices; i += 6)
		{
			indices[i] = _offset + 0;
			indices[i + 1] = _offset + 1;
			indices[i + 2] = _offset + 2;
			indices[i + 3] = _offset + 2;
			indices[i + 4] = _offset + 3;
			indices[i + 5] = _offset + 0;

			_offset += 4;
		}

		auto ibo = IndexBufferPtr(IndexBuffer::Create(indices, maxIndices));

		delete[] indices;

		vao = VertexArrayPtr(VertexArray::Create());

		vao->PushVertexBuffer(vbo);

		vao->SetIndexBuffer(ibo);
	}

	Renderer2D::Pipeline::~Pipeline()
	{
		delete[] pBufferHead;
	}

	void Renderer2D::Init()
	{
		s_hWhite1x1Texture = ResourceManagement::AddRuntimeResource
		(
			(Texture*)Texture::Create(new byte[4] { 255, 255, 255, 255 }, 1, 1, 4)
		);

		auto inBytes = (byte*)fallback_shader_source;
		s_hFallbackShader = ResourceManagement::AddRuntimeResource
		(
			(Shader*)Shader::Create
			(
				ResData(inBytes, inBytes + sizeof(fallback_shader_source)), 
				Shader::GetTemplateProps()
			)
		);

		RenderCommand::SetCullMode(WZ_CULL_NONE);
		RenderCommand::ToggleBlending(true);
	}
    void Renderer2D::Shutdown()
	{
		ResourceManagement::Delete(s_hWhite1x1Texture);
	}

	void Renderer2D::Begin(Pipeline* pipeline)
	{
		WZ_PROFILE_FUNCTION();
		WZ_CORE_ASSERT(!pipeline->ready, "Tried calling Begin() on already ready pipeline");
		pipeline->pBufferCurrent = pipeline->pBufferHead;
		pipeline->indexCount = 0;
		pipeline->slotCount = 0;
		pipeline->ready = true;
		pipeline->textureSlots.clear();
	}

	void Renderer2D::SubmitTexture(
		Pipeline* pipeline,
		const Texture::Handle& hTexture,  
		const vec2& position, 
		const vec2 scale, float rotation, 
		const Color& color, 
		const Rect& renderRect)
	{
		
		mat4 transform = glm::translate(mat4(1.f), vec3(position, 0.f));
		if (scale.x != 1 || scale.y != 1)
			transform = glm::scale(transform, vec3(scale, 1.f));
		if (rotation != 0)
			transform = glm::rotate(transform, glm::radians(rotation), vec3(0, 0, 1));

		SubmitTexture(pipeline, hTexture, transform, color, renderRect);
	}
	void Renderer2D::SubmitTexture(
		Pipeline* pipeline,
		const Texture::Handle& hTexture, 
		const mat4& transform, 
		const Color& color, 
		const Rect& renderRect)
	{
		WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hTexture), "Texture not loaded in Renderer2D")
		
		auto& texture = ResourceManagement::Get<Texture>(hTexture);

		if (pipeline->textureSlots.find(texture.GetId()) == pipeline->textureSlots.end())
		{
			pipeline->textureSlots[texture.GetId()] = pipeline->slotCount++;
		}

		u32 slot = pipeline->textureSlots[texture.GetId()];
		texture.Bind(slot);

		Submit
		(
			pipeline, 
			slot, 
			{ texture.GetWidth(), texture.GetHeight() }, 
			transform, 
			color, 
			renderRect
		);
	}

	void Renderer2D::SubmitRenderTarget(
		Pipeline* pipeline,
		const RenderTarget::Handle& hTexture,  
		const vec2& position, 
		const vec2 scale, float rotation, 
		const Color& color, 
		const Rect& renderRect)
	{
		mat4 transform = glm::translate(mat4(1.f), vec3(position, 0.f));
		if (scale.x != 1 || scale.y != 1)
			transform = glm::scale(transform, vec3(scale, 1.f));
		if (rotation != 0)
			transform = glm::rotate(transform, glm::radians(rotation), vec3(0, 0, 1));

		SubmitRenderTarget(pipeline, hTexture, transform, color, renderRect);
	}
	void Renderer2D::SubmitRenderTarget(
		Pipeline* pipeline,
		const RenderTarget::Handle& hTexture, 
		const mat4& transform, 
		const Color& color, 
		const Rect& renderRect)
	{
		WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hTexture), "Texture not loaded in Renderer2D")
		
		auto& texture = ResourceManagement::Get<RenderTarget>(hTexture);

		if (pipeline->textureSlots.find(texture.GetTextureId()) == pipeline->textureSlots.end())
		{
			pipeline->textureSlots[texture.GetTextureId()] = pipeline->slotCount++;
		}
		
		u32 slot = pipeline->textureSlots[texture.GetTextureId()];
		texture.BindTexture(slot);

		Submit
		(
			pipeline, 
			slot, 
			{ texture.GetWidth(), texture.GetHeight() }, 
			transform, 
			color, 
			renderRect
		);
	}

	void Renderer2D::SubmitRect(
		Pipeline* pipeline,
		const Rect& rect, 
		const Color& color, 
		RectMode mode, 
		const Rect& renderRect)
	{
		WZ_CORE_ASSERT(ResourceManagement::IsLoaded(s_hWhite1x1Texture), 
		"White texture not loaded properly");
		SubmitTexture(pipeline, s_hWhite1x1Texture, rect.position, rect.size, 0, color, renderRect);
	}

	void Renderer2D::End(Renderer2D::Pipeline* pipeline)
	{
		WZ_PROFILE_FUNCTION();
		WZ_CORE_ASSERT(pipeline->ready, "Renderer2D Begin()/End() mismatch");

		pipeline->ready = false;

		if (pipeline->indexCount == 0) return;

		pipeline->vbo->SetData
		(
			pipeline->pBufferHead, 
			(byte*)pipeline->pBufferCurrent - (byte*)pipeline->pBufferHead
		);

		Shader* shader;

		bool loaded = ResourceManagement::TryGet(pipeline->hShader, shader);

		if (!loaded)
		{
			WZ_CORE_ASSERT(ResourceManagement::IsLoaded(s_hFallbackShader), 
			"Fallback shader not loaded");
			shader = &ResourceManagement::Get<Shader>(s_hFallbackShader);
		}

		shader->Bind();
		shader->UploadMat4("u_camTransform", pipeline->camTransform);

		static const int slots[32] = 
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 
			17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 ,31 };
		shader->Upload1iv("u_textures", 32, slots);

		pipeline->vao->Bind();
		pipeline->vao->GetIndexBuffer()->Bind();

		RenderTarget* renderTarget = NULL;
		if (ResourceManagement::TryGet<RenderTarget>(pipeline->hRenderTarget, renderTarget))
		{
			renderTarget->Bind();
			RenderCommand::SetViewport
			(
				Viewport(0, 0, renderTarget->GetWidth(), renderTarget->GetHeight())
			);
		}

		RenderCommand::DrawIndexed(pipeline->vao, pipeline->indexCount);

		if (renderTarget) renderTarget->Unbind();

		s_metrics.numDrawCalls++;
	}

	void Renderer2D::Abort(Pipeline* pipeline)
	{
		pipeline->ready = false;
	}

	void Renderer2D::Clear(Renderer2D::Pipeline* pipeline)
	{
		WZ_PROFILE_FUNCTION();
		RenderTarget* pRenderTarget;
		ResourceManagement::TryGet<RenderTarget>(pipeline->hRenderTarget, pRenderTarget);

		if (pRenderTarget) pRenderTarget->Bind();
		RenderCommand::Clear();
		if (pRenderTarget) pRenderTarget->Unbind();
	}

	void Renderer2D::Submit(
		Pipeline* pipeline,
		u32 textureSlot,
		const vec2& size,
		mat4 transform,
		const Color& color,
		const Rect& renderRect)
	{
		WZ_PROFILE_FUNCTION();
		WZ_CORE_ASSERT(pipeline->ready, "Renderer not ready for rendertarget");

		if (pipeline->pBufferCurrent - pipeline->pBufferHead + 4 >= pipeline->maxObjects)
		{
			End(pipeline);
			Begin(pipeline);
		}

		static vec2 uv[] =
		{
			vec2(0, 0),
			vec2(1, 0),
			vec2(1, 1),
			vec2(0, 1)
		};
		static vec3 offsetTranslation(0.f);

		if (renderRect.IsValid())
		{
			float left = renderRect.Left() / size.x;
			float bot = renderRect.Bottom() / size.y;
			float right = renderRect.Right() / size.x;
			float top = renderRect.Top() / size.y;

			uv[0].x = left; uv[0].y = bot;
			uv[1].x = right; uv[1].y = bot;
			uv[2].x = right; uv[2].y = top;
			uv[3].x = left; uv[3].y = top;
		}

		auto& pBuffer = pipeline->pBufferCurrent;

		{
			WZ_PROFILE_SCOPE("Buffer assignment");
			// Bottom left
			pBuffer->pos = transform[3];
			pBuffer->uv = uv[0];
			pBuffer->color = color;
			pBuffer->textureSlot = textureSlot;
			pBuffer++;

			offsetTranslation.x = size.x;
			offsetTranslation.y = 0;
			transform = glm::translate(transform, offsetTranslation);

			// Bottom right
			pBuffer->pos = transform[3];
			pBuffer->uv = uv[1];
			pBuffer->color = color;
			pBuffer->textureSlot = textureSlot;
			pBuffer++;

			offsetTranslation.x = 0;
			offsetTranslation.y = size.y;
			transform = glm::translate(transform, offsetTranslation);

			// Top right
			pBuffer->pos = transform[3];
			pBuffer->uv = uv[2];
			pBuffer->color = color;
			pBuffer->textureSlot = textureSlot;
			pBuffer++;

			offsetTranslation.x = -size.x;
			offsetTranslation.y = 0;
			transform = glm::translate(transform, offsetTranslation);

			// Top left
			pBuffer->pos = transform[3];
			pBuffer->uv = uv[3];
			pBuffer->color = color;
			pBuffer->textureSlot = textureSlot;
			pBuffer++;
		}

		if (renderRect.IsValid())
		{
			uv[0].x = 0; uv[0].y = 0;
			uv[1].x = 1; uv[1].y = 0;
			uv[2].x = 1; uv[2].y = 1;
			uv[3].x = 0; uv[3].y = 1;
		}

		s_metrics.numQuads++;
		s_metrics.numIndices += 6;
		pipeline->indexCount += 6;

		if (pipeline->slotCount >= pipeline->numTextureSlots)
		{
			WZ_PROFILE_SCOPE("Premature flush");
			End(pipeline);
			Begin(pipeline);
		}
	}

}