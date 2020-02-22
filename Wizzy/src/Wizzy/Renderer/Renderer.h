#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Renderer/Buffers.h"
#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Renderer/RenderTarget.h"
#include "Wizzy/Renderer/RenderCommand.h"

namespace Wizzy {

    class Renderer
    {
    private:
        struct Submission
        {
            const VertexArrayPtr& va;
            Material material;
            mat4 transform;
        };
    public:
        
        static void Begin();

        static void Submit(const VertexArrayPtr& va, const Material& material, const mat4& transform);

        static void Flush(const mat4& cameraTransform = mat4(1), RenderTargetPtr renderTarget = NULL);

        inline static bool IsBusy() { return s_isBusy; }

        inline static
        void SetAPI(int8 api) { RendererAPI::SetAPI(api); }
        inline static
        int8 GetAPI() { return RendererAPI::GetAPI(); }

    private:
        typedef u32 RenderTargetId;

        static RenderTargetPtr s_renderTarget;
        static std::queue<Shader::Handle> s_shaderQueue;
        static Resource::HandleMap<std::queue<Submission>> s_submissionQueue;
        static bool s_isBusy;
    };

	//class Texture;

 //   struct Submission {
 //       VertexArrayPtr va;
 //       Material material;
 //       mat4 transform;
 //       RenderMode renderMode;

 //       Submission(VertexArrayPtr va, Material material, mat4 transform,
 //                  RenderMode renderMode)
 //           : va(va), material(material), transform(transform),
 //             renderMode(renderMode) {}
 //   };

 //   enum class LightType : int8 {
 //       NONE = -1,
 //       DIRECTIONAL, POINT, SPOT
 //   };

 //   struct Light {
 //       LightType type;
 //       vec3 position;
 //       vec3 rotation;
 //       Color color;
 //       float range;
 //       float intensity;
 //       float cutOff;
 //       float smoothness;

 //       Light(LightType type, const vec3& position = vec3(0), const vec3& rotation = vec3(0),
 //             const Color& color = Color::white, float range = 1, float intensity = 1, float cutOff = glm::radians(12.5f), float smoothness = .2f)
 //           : type(type), position(position), rotation(rotation),
 //             color(color), range(range), intensity(intensity), cutOff(cutOff), smoothness(smoothness) {}
 //   };

 //   struct RenderEnvironment {
 //       bool useLighting;
 //       Color ambient;
 //       Color clearColor;

 //       RenderEnvironment(bool useLighting = true,
 //                         const Color& ambient = Color::darkGrey,
 //                         const Color& clearColor = Color(.1f, .1f, .5f, 1.f))
 //           : useLighting(useLighting),  ambient(ambient), clearColor(clearColor) {}
 //   };

 //   class Renderer {
 //   public:
 //       static
 //       void Begin(const mat4& camTransform, const vec3& viewPos, const RenderEnvironment& environment = DEFAULT(RenderEnvironment));
 //       static
 //       void End();

 //       static
 //       void SubmitLight(LightType type,
 //                        const vec3& position,
 //                        const vec3& rotation,
 //                        const Color& color = Color(.80f, .85f, .85f, 1.f),
 //                        float range = 50.f,
 //                        float intensity = 1.f,
 //                        float cutOff = glm::radians(12.5f),
 //                        float smoothness = .2f);
 //       static
 //       void Submit(const VertexArrayPtr& va,
 //                   const Material& material,
 //                   const mat4& transform,
 //                   RenderMode mode = WZ_RENDER_MODE_TRIANGLES);
	//	/*static
	//	void Submit(const Texture& texture,
	//				const mat4& transform);*/

 //       inline static
 //       void SetAPI(int8 api) { RendererAPI::SetAPI(api); }
 //       inline static
 //       int8 GetAPI() { return RendererAPI::GetAPI(); }

 //       inline static
 //       void SetRenderTarget(const RenderTargetPtr& renderTarget) {
 //           s_renderTarget = renderTarget;
 //       }

 //       inline static
 //       const RenderTargetPtr& GetRenderTarget() {
 //           return s_renderTarget;
 //       }

 //   private:
 //       static
 //       void RenderSubmissions(std::deque<Submission>& submissions);

 //   private:
 //       static
 //       mat4                                                        s_camTransform;
 //       static
 //       vec3                                                        s_viewPos;
 //       static
 //       RenderEnvironment                                           s_environment;
 //       static
 //       std::vector<Light>                                          s_lights;
 //       static
 //       bool                                                        s_isReady;
 //       static
 //       std::unordered_map<Shader::Handle, std::deque<Submission>>  s_submissions;
 //       static
 //       ulib::Queue<Shader::Handle>                                 s_shaderQueue;
 //       static
 //       RenderTargetPtr                                             s_renderTarget;
 //   };
}
