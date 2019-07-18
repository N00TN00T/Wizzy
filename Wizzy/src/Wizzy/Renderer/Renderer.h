#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Renderer/RenderCommand.h"
#include "Wizzy/Renderer/Material.h"

namespace Wizzy {

    struct Submission {
        VertexArrayPtr va;
        Material material;
        mat4 transform;
        RenderMode renderMode;

        Submission(VertexArrayPtr va, Material material, mat4 transform,
                   RenderMode renderMode)
            : va(va), material(material), transform(transform),
              renderMode(renderMode) {}
    };

    enum class LightType : int8 {
        NONE = -1,
        DIRECTIONAL, POINT, SPOT
    };

    struct Light {
        LightType type;
        vec3 position;
        vec3 rotation;
        Color color;
        float range;
        float intensity;

        Light(LightType type, const vec3& position = vec3(0), const vec3& rotation = vec3(0),
              const Color& color = Color::white, float range = 1, float intensity = 1)
            : type(type), position(position), rotation(rotation),
              color(color), range(range), intensity(intensity) {}
    };

    struct RenderEnvironment {
        bool useLighting;
        Color ambient;

        RenderEnvironment(bool useLighting = true,
                          Color ambient = Color::darkGrey)
            : useLighting(useLighting),  ambient(ambient) {}
    };

    class Renderer {
    public:
        static
        void Begin(const mat4& camTransform, const vec3& viewPos, const RenderEnvironment& environment = DEFAULT(RenderEnvironment));
        static
        void End();

        static
        void SubmitLight(LightType type,
                         const vec3& position,
                         const vec3& rotation,
                         const Color& color = Color(.80f, .85f, .85f, 1.f),
                         float range = 50.f,
                         float intensity = 1.f);
        static
        void Submit(const VertexArrayPtr& va,
                    const Material& material,
                    const mat4& transform,
                    RenderMode mode = WZ_RENDER_MODE_TRIANGLES);

        inline static
        void SetAPI(int8 api) { RendererAPI::SetAPI(api); }
        inline static
        int8 GetAPI() { return RendererAPI::GetAPI(); }

    private:
        static
        void RenderSubmissions(std::deque<Submission>& submissions);

    private:
        static
        mat4 s_camTransform;
        static
        vec3 s_viewPos;
        static
        RenderEnvironment s_environment;
        static
        std::vector<Light> s_lights;
        static
        bool s_isReady;
        static
        std::unordered_map<ShaderHandle, std::deque<Submission>> s_submissions;
        static
        ulib::Queue<ShaderHandle> s_shaderQueue;
    };
}
