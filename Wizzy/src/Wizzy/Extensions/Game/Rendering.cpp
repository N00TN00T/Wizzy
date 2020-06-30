#include "wzpch.h"

#ifdef WZ_COMPILE_EXTENSIONS || defined(WZ_COMPILE_EXTENSION_GAME)

#include "Wizzy/Extensions/Game/Rendering.h"

constexpr char simple_2d_light_shader_source[] = 
R"(
#shader vertex

#version 330 core

layout (location = 0) in vec2 v_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec4 v_color;
layout (location = 3) in float v_location;

uniform mat4 u_camTransform;
uniform float u_ambient = 0.1;

out vec2 position;
out vec2 uv;
out vec4 color;
out float location;
out float ambient;

void main() {
    position = v_position;
    uv = v_uv;
	location = v_location;
	color = v_color;
    ambient = u_ambient;
    
    gl_Position = u_camTransform * vec4(v_position, 0.0, 1.0);
}


#shader fragment

#version 330 core

#define MAX_LIGHT_COUNT 16

out vec4 fragColor;


in vec2 position;
in vec2 uv;
in vec4 color;
in float location;
in float ambient;

struct PointLight
{
    vec2 lightPos;
    vec4 lightColor;
    float intensity;
    float radius;
};
uniform PointLight u_pointLights[MAX_LIGHT_COUNT];
uniform int u_pointLightCount = 0;

uniform sampler2D u_textures[32];

void main() {

    fragColor = vec4(ambient, ambient, ambient, 1.0);

    for (int i = 0; i < u_pointLightCount; i++)
    {
        float distRatio = max(1.0 - length((u_pointLights[i].lightPos - position) / (u_pointLights[i].radius * 2.0)), 0.0);

        fragColor += distRatio * u_pointLights[i].lightColor * u_pointLights[i].intensity;
    }    

    //fragColor = max(fragColor, vec4(vec3(ambient), 1.0));

    vec4 textureColor = texture(u_textures[int(round(location))], uv);

    fragColor *= color * textureColor;
}
)";

bool g_lightingEnabled = false;

WZ_NS_BEGIN(Wizzy);
WZ_NS_BEGIN(Game);

void SetLightingEnabled(bool enabled)
{
    g_lightingEnabled = enabled;
}
bool IsLightingEnabled()
{
    return g_lightingEnabled;
}

const Shader::Handle& FallbackShader2D()
{
    if (g_lightingEnabled)
    {
        return Get2DLightShader();
    } 
    else 
    {
        return Renderer2D::GetFallbackShader();
    }
}

const Shader::Handle& Get2DLightShader()
{
    static Shader::Handle s_h2DLightShader 
        = ResourceManagement::CreateRuntime<Shader>(simple_2d_light_shader_source);
    return s_h2DLightShader;
}

PipelinePool2D g_pipelines2D;

const PipelinePool2D& GetPipelinePool2D()
{
    return g_pipelines2D;
}

SpriteRenderer::SpriteRenderer()
{
    Flag<ColorTint>   (flag_optional);
    Flag<Scale2D>     (flag_optional);
    Flag<Rotation2D>  (flag_optional);
    Flag<CustomShader>(flag_optional);

    Subscribe(EventType::app_render);
}

bool SpriteRenderer::ProcessComponents(const Event& e, 
                                       Sprite* sprite,
                                       Position2D* position,
                                       ColorTint* colorTint,
                                       Scale2D* scale,
                                       Rotation2D* rotation,
                                       CustomShader* customShader
                                       ) const
{
    switch (e.GetEventType())
    {
        case EventType::app_render:
        {
            if (!ResourceManagement::IsLoaded(sprite->hTexture)) break;

            auto& pipelines = g_pipelines2D[FallbackShader2D()];
            
            if (customShader && ResourceManagement::IsLoaded(customShader->hShader))
            {
                WZ_ASSERT(g_pipelines2D.find(customShader->hShader) != g_pipelines2D.end(),
                            "Untracked shader found in SpriteRenderer");
                pipelines = g_pipelines2D[customShader->hShader];
            }

            for (auto pipeline : pipelines)
            {
                if (!pipeline->IsReady()) continue;
                wz::Renderer2D::SubmitTexture
                (
                    pipeline, 
                    sprite->hTexture,
                    position->value,
                    scale ? scale->value : fvec2(1),
                    rotation ? rotation->value : 0,
                    colorTint ? colorTint->value : COLOR_WHITE
                );
            }

            break;
        }
    }

    return false;
}


CameraSystem2D::CameraSystem2D()
{
    Flag<Rotation2D>(flag_optional);
    Flag<Scale2D>(flag_optional);
    Flag<ColorTint>(flag_optional);

    Subscribe(EventType::app_update);
    Subscribe(EventType::app_frame_end);
    Subscribe(EventType::resource_load);
    Subscribe(EventType::resource_unload);
}

bool CameraSystem2D::ProcessComponents(const Event& e, 
                                       Camera2D* camera,
                                       Position2D* position,
                                       Rotation2D* rotation,
                                       Scale2D* scale,
                                       ColorTint* colorTint
                                       ) const
{
    switch (e.GetEventType())
    {
        case EventType::app_update:
        {
            ResourceManagement::ForEach<Shader>([&] (const auto& hnd) 
            {
                auto& info = ResourceManagement::GetInfoFor(hnd);
                if (wz::ResourceManagement::IsLoaded(hnd) 
                    && camera->pipelines.find(hnd) == camera->pipelines.end())
                {
                    RegisterShader(camera, hnd);
                }
                return true;
            });

            if (!camera->consistentSize)
            {
                UpdateSize(camera);
            }

            UpdateTransform(camera, position, scale, rotation);

            for (auto& [hShader, pipeline] : camera->pipelines)
            {
                pipeline->hRenderTarget = camera->hRenderTarget;
                pipeline->hShader = hShader;
                if (colorTint)
                {
                    pipeline->clearColor = colorTint->value;
                }

                pipeline->camTransform = camera->transform;

                Renderer2D::Clear(pipeline);
                Renderer2D::Begin(pipeline);
            }

            break;
        }

        case EventType::app_frame_end:
        {
            for (auto& [hShader, pipeline] : camera->pipelines)
            {
                if (!pipeline->IsReady()) continue;
                Renderer2D::End(pipeline);
            }
            break;
        }
        case EventType::resource_load:
        {
            /*auto& loadEvent = (const ResourceLoadedEvent&)e;
            auto& hnd = loadEvent.GetResourceHandle();
            
            if (ResourceManagement::Is<Shader>(hnd))
            {
                RegisterShader(camera, hnd);
            }*/

            break;
        }
        case EventType::resource_unload:
        {
           /* auto& unloadEvent = (const ResourceUnloadedEvent&)e;
            auto& hnd = unloadEvent.GetResourceHandle();*/
            
            break;
        }
        
    }

    return false;
}

void CameraSystem2D::RegisterShader(Camera2D* camera, const Shader::Handle& hShader) const
{
    WZ_EXT_TRACE("Registering shader in camera");

    // TODO: (#OPTIMIZE, #MEMORY, #DANGER)
    // A lot of shaders may be loaded without being used,
    // which means a lot of memory will be allocated for pipelines
    // that aren't used. This is potentially dangerous as it may fill
    // up the memory very fast if there are a lot of cameras and shaders
    // in the app.
    // Potential fix: Start off with a few bytes and reallocate the
    // pipeline with bigger size dynamiclly if it's used often.
    // The pipeline should probably have a Resize() function
    // so we don't need to deallocate the pipeline and leave
    // garbage memory in the global pipeline pool
    // Another fix could be to unload shaders from ram and save
    // them on the disk and load them when they're used. The camera
    // doesn't register shaders with pipelines if they aren't in RAM.
    auto pipeline = new RenderPipeline2D(_MB(5));
    pipeline->hRenderTarget = camera->hRenderTarget;
    pipeline->hShader = hShader;
    g_pipelines2D[hShader].emplace(pipeline);
    camera->pipelines[hShader] = pipeline;
}

void CameraSystem2D::UnregisterShader(Camera2D* camera, const Shader::Handle& hShader) const
{
    WZ_EXT_TRACE("Unregistering shader in camera");

    WZ_ASSERT(camera->pipelines.find(hShader) != camera->pipelines.end() 
                && camera->pipelines[hShader] != NULL,
                    "Untracked shader in camera");
    auto pipeline = camera->pipelines[hShader];
    WZ_ASSERT(g_pipelines2D[hShader].find(pipeline) != g_pipelines2D[hShader].end(),
                    "Camera pipeline not in global pipeline pool");

    g_pipelines2D[hShader].erase(pipeline);
    camera->pipelines.erase(hShader);
    delete pipeline;
}

void CameraSystem2D::UpdateSize(Camera2D* camera) const
{
    if (ResourceManagement::IsLoaded(camera->hRenderTarget))
    {
        auto& renderTarget = ResourceManagement::Get<RenderTarget>(camera->hRenderTarget);
        camera->viewSize.x = renderTarget.GetWidth();
        camera->viewSize.y = renderTarget.GetHeight();
    }
    else 
    {
        auto& wnd = Application::Get().GetWindow();
        camera->viewSize.x = wnd.GetWidth();
        camera->viewSize.y = wnd.GetHeight();
    }
}

void CameraSystem2D::UpdateTransform(Camera2D* camera, 
                                     Position2D* position, 
                                     Scale2D* scale, 
                                     Rotation2D* rotation
                                     ) const
{
    static fmat4 projection;
    projection = projection::ortho<float>
                (
                    0, 
                    camera->viewSize.x * (scale ? scale->value.x : 1.f), 
                    0, 
                    camera->viewSize.y * (scale ? scale->value.y : 1.f),
                    -1,
                    1
                );

    static fmat4 view;
    view = transformation::translation((fvec3)position->value);
    if (rotation)
    {
        view.rotate(rotation->value, fvec3(0, 0, 1));
    }
    view.invert();

    camera->transform = projection * view;
}

TextRenderer::TextRenderer()
{
    Flag<Rotation2D>(flag_optional);
    Flag<Scale2D>(flag_optional);
    Flag<ColorTint>(flag_optional);
    Flag<CustomShader>(flag_optional);

    Subscribe(EventType::app_render);
}

bool TextRenderer::ProcessComponents(const Event& e, 
                                     Text* text,
                                     Position2D* position,
                                     Rotation2D* rotation,
                                     Scale2D* scale,
                                     ColorTint* colorTint,
                                     CustomShader* customShader
                                     ) const
{
    switch (e.GetEventType())
    {
        case EventType::app_render:
        {
            if (!ResourceManagement::IsLoaded(text->hFont)) return false;

            auto& pipelines = g_pipelines2D[Renderer2D::GetTextShader()];

            if (customShader && ResourceManagement::IsLoaded(customShader->hShader))
            {
                WZ_ASSERT(g_pipelines2D.find(customShader->hShader) != g_pipelines2D.end(),
                            "Untracked shader found in TextRenderer");
                pipelines = g_pipelines2D[customShader->hShader];
            }
            
            for (auto& pipeline : pipelines)
            {
                Renderer2D::SubmitText
                (
                    pipeline,
                    text->hFont,
                    text->value,
                    position->value,
                    scale ? scale->value : fvec2(1.f),
                    rotation ? rotation->value : 0.f,
                    colorTint ? colorTint->value : COLOR_WHITE
                );
            }

            break;
        }
    }   

    return false;
}



LightRenderer2D::LightRenderer2D()
{
    Subscribe(EventType::app_frame_begin);
    Subscribe(EventType::app_update);

    Flag<CustomShader>(flag_optional);
    Flag<ColorTint>(flag_optional);
    Flag<LightOffset>(flag_optional);
}

bool LightRenderer2D::ProcessComponents(const Event& e,
                                      Position2D* position,
                                      Light* light,
                                      ColorTint* colorTint,
                                      CustomShader* customShader,
                                      LightOffset* offset
                                      ) const
{
    // TODO: Thread safe
    static u32 count = 1;

    switch (e.GetEventType())
    {
        case EventType::app_frame_begin:
        {
            count = 1;
            break;
        }
        case EventType::app_update:
        {
            // TODO: Custom shader
            auto& hShader = FallbackShader2D();

            if (wz::ResourceManagement::IsLoaded(hShader))
            {
                auto& shader = wz::ResourceManagement::Get<wz::Shader>(hShader);

                // TODO: UBO (#OPTIMIZE)
                string name = "u_pointLights[" + std::to_string(count - 1) + "]";

                fvec2 pos = position->value + (offset ? offset->value : fvec2(0));

                shader.Upload2f(name + ".lightPos", pos);
                shader.Upload4f(name + ".lightColor", colorTint ? colorTint->value : COLOR_WHITE);
                shader.Upload1f(name + ".intensity", light->intensity);
                shader.Upload1f(name + ".radius", light->radius);
                
                shader.Upload1i("u_pointLightCount", count++);
            }

            break;
        }
    }

    return false;
}                                      

WZ_NS_END;
WZ_NS_END;

#endif