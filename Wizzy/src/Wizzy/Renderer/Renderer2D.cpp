#include "wzpch.h"

#include "Wizzy/Renderer/Renderer2D.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Renderer/RenderCommand.h"

#include "Wizzy/Instrumentor.h"
#include "Wizzy/WizzyExceptions.h"


namespace Wizzy
{
	Renderer2D::RenderTargetData Renderer2D::s_windowTargetData;
	std::unordered_map<RenderTarget::Handle, Renderer2D::RenderTargetData, Resource::Handle::hash> Renderer2D::s_renderTargetData;

	size_t Renderer2D::TEXTURE_SLOTS;
	
	void Renderer2D::Begin(Shader::Handle hShader, const mat4& cameraTransform, RenderTarget::Handle hRenderTarget)
	{
		RenderTargetData& tData = ResourceManagement::IsLoaded(hRenderTarget) ? s_renderTargetData[hRenderTarget] : s_windowTargetData;

		tData.camTransform = cameraTransform;

		WZ_PROFILE_FUNCTION();
		if (!tData.initialized) Init(hRenderTarget);

		{
			WZ_PROFILE_SCOPE("Mapping buffer");
			tData.vao->Bind();
			tData.vbo->Bind();
			tData.pBufferData = (VertexData*)tData.vbo->Map();
		}

		{
			WZ_PROFILE_SCOPE("Begin Misc");
			WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hShader), "Shader isnt loaded Renderer2D");

			tData.hShader = hShader;

			tData.slotCount = 0;
			tData.indexCount = 0;
			tData.submissionCount = 0;

			tData.ready = true;
		}
	}

	void Renderer2D::SubmitImage(Texture::Handle hTexture, const glm::vec2& position, const glm::vec2 scale, float rotation, const glm::vec4& color, RenderTarget::Handle hRenderTarget)
	{
		mat4 transform = glm::mat4(1.f);
		{
			WZ_PROFILE_SCOPE("Transformation");
			transform = glm::translate(transform, vec3(position, 0.f));
			if (scale.x != 1 || scale.y != 1)
				transform = glm::scale(transform, vec3(scale, 1.f));
			if (rotation != 0)
				transform = glm::rotate(transform, glm::radians(rotation), vec3(0, 0, 1));
		}
		SubmitImage(hTexture, transform, color, hRenderTarget);
	}

	void Renderer2D::SubmitImage(Texture::Handle hTexture, mat4 transform, const glm::vec4& color, RenderTarget::Handle hRenderTarget)
	{
		WZ_PROFILE_FUNCTION();

		WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hTexture), "Texture not loaded in Renderer2D")

			RenderTargetData& tData = ResourceManagement::IsLoaded(hRenderTarget) ? s_renderTargetData[hRenderTarget] : s_windowTargetData;

		WZ_CORE_ASSERT(tData.submissionCount <= MAX_OBJECTS, "Renderer out of memory (" + std::to_string((tData.submissionCount- 1) * OBJECT_SIZE) + " bytes used of " + std::to_string(MEMORY_BUDGET) + ")");

		auto& texture = ResourceManagement::Get<Texture>(hTexture);

		if (tData.textureSlots.find(texture.GetId()) == tData.textureSlots.end())
		{
			tData.textureSlots[texture.GetId()] = tData.slotCount++;
		}

		u32 slot = tData.textureSlots[texture.GetId()];
		texture.Bind(slot);

		Submit(slot, { texture.GetWidth(), texture.GetHeight() }, transform, color, hRenderTarget);
	}

	void Renderer2D::SubmitImage(RenderTarget::Handle hTexture, const glm::vec2& position, const glm::vec2 scale, float rotation, const glm::vec4& color, RenderTarget::Handle hRenderTarget)
	{
		mat4 transform = glm::mat4(1.f);
		{
			WZ_PROFILE_SCOPE("Transformation");
			transform = glm::translate(transform, vec3(position, 0.f));
			if (scale.x != 1 || scale.y != 1)
				transform = glm::scale(transform, vec3(scale, 1.f));
			if (rotation != 0)
				transform = glm::rotate(transform, glm::radians(rotation), vec3(0, 0, 1));
		}
		SubmitImage(hTexture, transform, color, hRenderTarget);
	}
	void Renderer2D::SubmitImage(RenderTarget::Handle hTexture, mat4 transform, const glm::vec4& color, RenderTarget::Handle hRenderTarget)
	{
		WZ_PROFILE_FUNCTION();

		RenderTargetData& tData = ResourceManagement::IsLoaded(hRenderTarget) ? s_renderTargetData[hRenderTarget] : s_windowTargetData;

		WZ_CORE_ASSERT(tData.submissionCount <= MAX_OBJECTS, "Renderer out of memory (" + std::to_string((tData.submissionCount - 1) * OBJECT_SIZE) + " bytes used of " + std::to_string(MEMORY_BUDGET) + ")");

		auto& texture = ResourceManagement::Get<RenderTarget>(hRenderTarget);

		if (tData.textureSlots.find(texture.GetTextureId()) == tData.textureSlots.end())
		{
			tData.textureSlots[texture.GetTextureId()] = tData.slotCount++;
		}

		u32 slot = tData.textureSlots[texture.GetTextureId()];
		texture.BindTexture(slot);

		Submit(slot, { texture.GetWidth(), texture.GetHeight() }, transform, color, hRenderTarget);
	}

	void Renderer2D::SubmitText(const string& text, Font* font, const glm::vec2& position, const glm::vec2 scale, float rotation, const glm::vec4& color, RenderTarget::Handle hRenderTarget)
	{
		mat4 transform = glm::mat4(1.f);
		{
			WZ_PROFILE_SCOPE("Transformation");
			transform = glm::translate(transform, vec3(position, 0.f));
			if (scale.x != 1 || scale.y != 1)
				transform = glm::scale(transform, vec3(scale, 1.f));
			if (rotation != 0)
				transform = glm::rotate(transform, glm::radians(rotation), vec3(0, 0, 1));
		}
		SubmitText(text, font, transform, color, hRenderTarget);
	}

	void Renderer2D::SubmitText(const string& text, Font* font, mat4 transform, const glm::vec4& color, RenderTarget::Handle hRenderTarget)
	{
		WZ_PROFILE_FUNCTION();

		WZ_CORE_ASSERT(font != nullptr, "Font is null");

		RenderTargetData& tData = ResourceManagement::IsLoaded(hRenderTarget) ? s_renderTargetData[hRenderTarget] : s_windowTargetData;

		WZ_CORE_ASSERT(tData.submissionCount <= MAX_OBJECTS, "Renderer out of memory (" + std::to_string((tData.submissionCount - 1) * OBJECT_SIZE) + " bytes used of " + std::to_string(MEMORY_BUDGET) + ")");

		RenderTarget::Handle hTexture = font->Render(text, tData.hShader);

		auto& texture = ResourceManagement::Get<RenderTarget>(hTexture);

		if (tData.textureSlots.find(texture.GetTextureId()) == tData.textureSlots.end())
		{
			tData.textureSlots[texture.GetTextureId()] = tData.slotCount++;
		}

		u32 slot = tData.textureSlots[texture.GetTextureId()];
		texture.BindTexture(slot);

		Submit(slot, { texture.GetWidth(), texture.GetHeight() }, transform, color, hRenderTarget);
	}

	void Renderer2D::SubmitRect(const Rect& rect, const glm::vec4& color, RectMode mode, RenderTarget::Handle hRenderTarget)
	{
		RenderTargetData& tData = ResourceManagement::IsLoaded(hRenderTarget) ? s_renderTargetData[hRenderTarget] : s_windowTargetData;
		WZ_CORE_ASSERT(ResourceManagement::IsLoaded(tData.hWhiteTexture), "White texture not loaded properly");
		SubmitImage(tData.hWhiteTexture, rect.position, rect.size, 0, color, hRenderTarget);
	}

	void Renderer2D::End( RenderTarget::Handle hRenderTarget)
	{
		WZ_PROFILE_FUNCTION();

		RenderTargetData& tData = ResourceManagement::IsLoaded(hRenderTarget) ? s_renderTargetData[hRenderTarget] : s_windowTargetData;

		WZ_CORE_ASSERT(tData.ready, "Renderer not ready for rendertarget");

		tData.vbo->Unmap();
		tData.pBufferData = NULL;
		tData.ready = false;

		WZ_CORE_ASSERT(ResourceManagement::IsLoaded(tData.hShader), "Shader not loaded renderer");

		auto& shader = ResourceManagement::Get<Shader>(tData.hShader);
		shader.Bind();
		shader.UploadMat4("u_camTransform", tData.camTransform);

		static const int slots[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 ,31 };
		shader.Upload1iv("u_textures", 32, slots);

		tData.vao->Bind();
		tData.vao->GetIndexBuffer()->Bind();

		RenderTarget* renderTarget = NULL;
		if (ResourceManagement::TryGet<RenderTarget>(hRenderTarget, renderTarget))
		{
			renderTarget->Bind();
			RenderCommand::Clear();
			RenderCommand::SetViewport(Viewport(0, 0, renderTarget->GetWidth(), renderTarget->GetHeight()));
		}

		RenderCommand::DrawIndexed(tData.vao, tData.indexCount);

		if (renderTarget) renderTarget->Unbind();

		tData.textureSlots.clear();
	}

	bool Renderer2D::IsReady(RenderTarget::Handle hRenderTarget)
	{
		RenderTargetData& tData = ResourceManagement::IsLoaded(hRenderTarget) ? s_renderTargetData[hRenderTarget] : s_windowTargetData;
		return tData.ready;
	}

	void Renderer2D::Init(RenderTarget::Handle hRenderTarget)
	{
		WZ_PROFILE_FUNCTION();

		RenderTargetData& tData = ResourceManagement::IsLoaded(hRenderTarget) ? s_renderTargetData[hRenderTarget] : s_windowTargetData;

		TEXTURE_SLOTS = MaxTextureSlot() + 1ULL;

		tData.vbo = VertexBufferPtr(VertexBuffer::Create(NULL, MEMORY_BUDGET, WZ_BUFFER_USAGE_DYNAMIC_DRAW));
		tData.vbo->Bind();

		BufferLayout layout = {
			{ ShaderDataType::FLOAT2, "v_position" },
			{ ShaderDataType::FLOAT2, "v_uv" },
			{ ShaderDataType::FLOAT4, "v_color" },
			{ ShaderDataType::FLOAT1, "v_location", true },
		};

		tData.vbo->SetLayout(layout);

		u32* _indices = new u32[MAX_INDICES];

		int _offset = 0;
		for (int i = 0; i < MAX_INDICES; i += 6)
		{
			_indices[i]		= _offset + 0;
			_indices[i + 1] = _offset + 1;
			_indices[i + 2] = _offset + 2;
			_indices[i + 3] = _offset + 2;
			_indices[i + 4] = _offset + 3;
			_indices[i + 5] = _offset + 0;

			_offset += 4;
		}

		auto ibo = IndexBufferPtr(IndexBuffer::Create(_indices, MAX_INDICES));

		delete[] _indices;

		tData.vao = VertexArrayPtr(VertexArray::Create());
		tData.vao->Bind();

		tData.vao->PushVertexBuffer(tData.vbo);

		tData.vao->SetIndexBuffer(ibo);

		if (!ResourceManagement::IsLoaded(tData.hWhiteTexture))
		{
			tData.hWhiteTexture = ResourceManagement::AddRuntimeResource(Texture::Create(new byte[4]{ 255, 255, 255, 255 }, 1, 1, 4), Texture::GetTemplateProps());
		}

		WZ_CORE_ASSERT(ResourceManagement::IsLoaded(tData.hWhiteTexture), "White texture handle is not correctly loaded");

		tData.initialized = true;
	}

	void Renderer2D::Submit(u32 slot, const vec2& size, mat4 transform, const glm::vec4& color, RenderTarget::Handle hRenderTarget)
	{
		RenderTargetData& tData = ResourceManagement::IsLoaded(hRenderTarget) ? s_renderTargetData[hRenderTarget] : s_windowTargetData;
		
		WZ_CORE_ASSERT(tData.ready, "Renderer not ready for rendertarget");

		VertexData* pBuffer = NULL;

		pBuffer = tData.pBufferData;
		tData.pBufferData += 4;
		tData.submissionCount++;

		// Cache to drastcally decrease amount of constructions and dynamic allocations
		static const vec2 uv[] =
		{
			vec2(0, 0),
			vec2(1, 0),
			vec2(1, 1),
			vec2(0, 1)
		};
		static vec3 offsetTranslation(0.f);

		{
			WZ_PROFILE_SCOPE("Buffer assignment");
			// Bottom left
			pBuffer->pos = transform[3];
			pBuffer->uv = uv[0];
			pBuffer->color = color;
			pBuffer->textureSlot = slot;
			pBuffer++;

			offsetTranslation.x = size.x;
			offsetTranslation.y = 0;
			transform = glm::translate(transform, offsetTranslation);

			// Bottom right
			pBuffer->pos = transform[3];
			pBuffer->uv = uv[1];
			pBuffer->color = color;
			pBuffer->textureSlot = slot;
			pBuffer++;

			offsetTranslation.x = 0;
			offsetTranslation.y = size.y;
			transform = glm::translate(transform, offsetTranslation);

			// Top right
			pBuffer->pos = transform[3];
			pBuffer->uv = uv[2];
			pBuffer->color = color;
			pBuffer->textureSlot = slot;
			pBuffer++;

			offsetTranslation.x = -size.x;
			offsetTranslation.y = 0;
			transform = glm::translate(transform, offsetTranslation);

			// Top left
			pBuffer->pos = transform[3];
			pBuffer->uv = uv[3];
			pBuffer->color = color;
			pBuffer->textureSlot = slot;
		}

		tData.indexCount += 6;

		if (tData.slotCount >= TEXTURE_SLOTS)
		{
			WZ_PROFILE_SCOPE("Premature flush");
			End(hRenderTarget);
			Begin(tData.hShader, tData.camTransform, hRenderTarget);
			tData.slotCount = 0;
		}
	}

}