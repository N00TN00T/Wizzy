#include "wzpch.h"

#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Stopwatch.h"

namespace Wizzy {

    RenderTargetPtr Renderer::s_renderTarget;
    std::queue<Shader::Handle> Renderer::s_shaderQueue;
    Resource::HandleMap<std::queue<Renderer::Submission>> Renderer::s_submissionQueue;
    bool Renderer::s_isBusy;

    void Renderer::Begin()
    {
    }

    void Renderer::Submit(const VertexArrayPtr& va, const Material& material, const mat4& transform)
    {
        WZ_CORE_ASSERT(va, "Vertex array cannot be null");

        s_shaderQueue.push(material.GetShader());
        s_submissionQueue[material.GetShader()].push({ va, material, transform });
    }

    void Renderer::Flush(const mat4& cameraTransform, RenderTargetPtr renderTarget)
    {
        s_isBusy = true;
        if (renderTarget) renderTarget->Bind();

        while (!s_shaderQueue.empty())
        {
            auto hShader = s_shaderQueue.front();
            WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hShader), "Shader is not loaded when flushing");

            Shader& shader = ResourceManagement::Get<Shader>(hShader);
            shader.Bind();

            shader.UploadMat4("u_camTransform", cameraTransform);

            auto& submissionQueue = s_submissionQueue[hShader];

            while (!submissionQueue.empty())
            {
                auto& submission = submissionQueue.front();

                shader.UploadMat4("u_transform", submission.transform);

                RenderCommand::DrawIndexed(submission.va, submission.va->GetIndexBuffer()->GetCount());

                submissionQueue.pop();
            }

            s_shaderQueue.pop();
        }

        if (renderTarget) renderTarget->Bind();

        s_isBusy = false;
    }

	//VertexArrayPtr g_quadVao;
	//Material g_quadMaterial = Material(WZ_DEFAULT_SHADER_HANDLE);

 //   mat4 Renderer::s_camTransform;
 //   vec3 Renderer::s_viewPos;
 //   RenderEnvironment Renderer::s_environment;
 //   std::vector<Light> Renderer::s_lights;
 //   bool Renderer::s_isReady(false);
 //   std::unordered_map<Shader::Handle, std::deque<Submission>> Renderer::s_submissions;
 //   ulib::Queue<Shader::Handle> Renderer::s_shaderQueue;
 //   RenderTargetPtr Renderer::s_renderTarget;

 //   void Renderer::Begin(const mat4& camTransform, const vec3& viewPos, const RenderEnvironment& environment) {
 //       WZ_CORE_TRACE("Beginning renderer...");
 //       s_camTransform = camTransform;
 //       s_viewPos = viewPos;

 //       s_environment = environment;
 //       s_isReady = true;

 //       RenderCommand::SetClearColor(environment.clearColor.asVec4);

	//	if (!g_quadVao) {
	//		g_quadVao = VertexArrayPtr(VertexArray::Create());

	//		float _verts[4 * 3] = {
	//			-.5f, -.5f, 0.f,	// Bot left		0
	//			 .5f, -.5f, 0.f,	// Bot right	1
	//			 .5f,  .5f, 0.f		// Top right	2
	//			-.5f,  .5f, 0.f,	// Top left		3
	//		};
	//		u32 _indices[6] = {
	//			0, 1, 2,
	//			2, 3, 0,
	//		};

	//		BufferLayout _layout = {
	//			{ ShaderDataType::FLOAT3, "position" }
	//		};

	//		VertexBufferPtr _vbo = VertexBufferPtr(VertexBuffer::Create(_verts, 4 * 3 * sizeof(float)));
	//		_vbo->SetLayout(_layout);

	//		IndexBufferPtr _ibo = IndexBufferPtr(IndexBuffer::Create(_indices, 6));

	//		g_quadVao->PushVertexBuffer(_vbo);
	//		g_quadVao->SetIndexBuffer(_ibo);
	//	}
 //   }
 //   void Renderer::End() {
 //       WZ_CORE_ASSERT(s_isReady, "Begin() was not called on Renderer before End()");
 //       WZ_CORE_TRACE("Flushing renderer...");

 //       if (s_renderTarget) s_renderTarget->Bind();

 //       while (!s_shaderQueue.IsEmpty()) {

 //           auto _shaderHandle = s_shaderQueue.Pop();
 //           Shader& _shader = ResourceManagement::Get<Shader>(_shaderHandle);
 //           auto& _submissions = s_submissions[_shaderHandle];

 //           _shader.Bind();
 //           _shader.UploadMat4("u_camTransform", s_camTransform);
 //           _shader.Upload3f("u_viewPos", s_viewPos);

 //           if (s_environment.useLighting) {
 //               _shader.Upload4f("u_ambient", s_environment.ambient.asVec4);

 //               _shader.Upload1i("u_nLights", s_lights.size());

 //               for (u32 i = 0; i < s_lights.size(); i++) {
 //                   _shader.Upload1i("u_lights[" + std::to_string(i) + "].type", (int32)s_lights[i].type);
 //                   _shader.Upload3f("u_lights[" + std::to_string(i) + "].position", s_lights[i].position);
 //                   _shader.Upload3f("u_lights[" + std::to_string(i) + "].direction", s_lights[i].rotation);
 //                   _shader.Upload4f("u_lights[" + std::to_string(i) + "].color", s_lights[i].color.asVec4);
 //                   _shader.Upload1f("u_lights[" + std::to_string(i) + "].range", s_lights[i].range);
 //                   _shader.Upload1f("u_lights[" + std::to_string(i) + "].intensity", s_lights[i].intensity);
 //                   _shader.Upload1f("u_lights[" + std::to_string(i) + "].cutOff", glm::cos(s_lights[i].cutOff));
 //                   _shader.Upload1f("u_lights[" + std::to_string(i) + "].smoothness", glm::cos(s_lights[i].smoothness));
 //               }
 //           }

 //           WZ_CORE_TRACE("Renderings submissions submitted with shader '{0}'", _shaderHandle);
 //           RenderSubmissions(_submissions);

 //           _submissions.clear();
 //       }

 //       s_lights.clear();

 //       s_isReady = false;

 //       if (s_renderTarget) s_renderTarget->Unbind();
 //   }

 //   void Renderer::SubmitLight(LightType type,
 //                              const vec3& position,
 //                              const vec3& rotation,
 //                              const Color& color,
 //                              float range,
 //                              float intensity,
 //                              float cutOff,
 //                              float smoothness) {
 //       WZ_CORE_TRACE("Submitting light...");
 //       s_lights.push_back({type, position, rotation, color, range, intensity, cutOff, smoothness});
 //   }

 //   void Renderer::Submit(const VertexArrayPtr& va,
 //                           const Material& material,
 //                           const mat4& transform,
 //                           RenderMode mode) {
 //       WZ_CORE_ASSERT(s_isReady, "Begin() was not called on Renderer before Submit()");
 //       WZ_CORE_ASSERT(ResourceManagement::Is<Shader>(material.GetShaderHandle()), "Shader was either null handle or wrong type in renderer submit");
 //       WZ_CORE_TRACE("Pushing 3D submission  to renderer...");
 //       s_shaderQueue.Push(material.GetShaderHandle());
 //       s_submissions[material.GetShaderHandle()].push_back({ va, material, transform, mode });
 //   }

	///*void Renderer::Submit(const Texture& texture, const mat4& transform) {
	//	WZ_CORE_ASSERT(s_isReady, "Begin() was not called on Renderer before Submit()");
	//	WZ_CORE_ASSERT(ResourceManagement::Is<Shader>(g_quadMaterial.GetShaderHandle()), "Quad shader not in resource management");
	//	WZ_CORE_TRACE("Pushing 2D submission  to renderer...");

	//	s_shaderQueue.Push(g_quadMaterial.GetShaderHandle());
	//	s_submissions[g_quadMaterial.GetShaderHandle()].push_back({ g_quadVao, g_quadMaterial, transform, WZ_RENDER_MODE_TRIANGLES });
	//}*/

 //   void Renderer::RenderSubmissions(std::deque<Submission>& submissions) {
 //       for (auto& _submission : submissions) {
 //           auto& _material = _submission.material;
	//		WZ_CORE_ASSERT(ResourceManagement::IsReadyAs<Shader>(_material.GetShaderHandle()), "Shader was not ready in renderer");
 //           auto& _shader = ResourceManagement::Get<Shader>(_material.GetShaderHandle());
 //           _material.Bind();
 //           _submission.va->Bind();
 //           _submission.va->GetIndexBuffer()->Bind();
 //           _shader.UploadMat4("u_worldTransform", _submission.transform);

 //           RenderCommand::DrawIndexed(_submission.va, _submission.renderMode);


//           _submission.va->Unbind();
 //           _submission.va->GetIndexBuffer()->Unbind();
 //       }
 //   }
}
