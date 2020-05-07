#include "spch.h"

#include "DemoGame2D.h"

#define COMPONENT_DEF(n) struct n : public wz::Component<n>


wz::Renderer2D::Pipeline* g_mainPipeline;
wz::Renderer2D::Pipeline* g_textPipeline;
u16 g_lightCounter = 0;
wz::ECSManager* g_ecs;
bool g_drawColliders = false;

COMPONENT_DEF(Position)
{
    vec2 value = vec2(0, 0);
};

COMPONENT_DEF(Sprite)
{
    wz::Texture::Handle hTexture = WZ_NULL_RESOURCE_HANDLE;
};

COMPONENT_DEF(PlayerConfig)
{   
    float speed = 500.f;
};

COMPONENT_DEF(Brick)
{
    
};

COMPONENT_DEF(Powerup)
{

};

COMPONENT_DEF(CollisionBox)
{
    wz::Rect rectangle = wz::Rect(0, 0, 32, 32);
};

COMPONENT_DEF(Ball)
{
    vec2 direction;
    float speed = 650.f;
    vec2 spawnPos;
};

COMPONENT_DEF(Camera)
{
    mat4 projection;
};

COMPONENT_DEF(Gui)
{
    bool showMetrics = false;
    bool showGameDebugging = false;
};

COMPONENT_DEF(Time)
{
    float value;
};

COMPONENT_DEF(Text)
{
    wz::Font::Handle hFont;
    string value;
};

COMPONENT_DEF(ResourceComponent)
{
    
};

COMPONENT_DEF(ColorTint)
{
    wz::Color value = wz::Color::white;
};

COMPONENT_DEF(LightConfig)
{
    float intensity = 1.f;
	float ambient = .1f;
	float radius = 100.f;
};

class PlayerController : public wz::System
{
public:
    PlayerController()
    {
        AddComponentType<PlayerConfig>();
        AddComponentType<Position>();
        AddComponentType<Sprite>();

        Subscribe(wz::EventType::app_init);
        Subscribe(wz::EventType::app_update);
    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& playerConfig = *components.Get<PlayerConfig>();
        auto& position = *components.Get<Position>();
        auto& sprite = *components.Get<Sprite>();

        switch (e.GetEventType())
        {
            case wz::EventType::app_init:
            {
                return;
            }
            case wz::EventType::app_update:
            {
                auto& updateEvent = (const wz::AppUpdateEvent&)e;
                float delta = updateEvent.GetDeltaTime();

                if (wz::Input::GetKey(WZ_KEY_A))
                {   
                    position.value.x -= delta * playerConfig.speed;
                }
                if (wz::Input::GetKey(WZ_KEY_D))
                {
                    position.value.x += delta * playerConfig.speed;
                }

                auto& texture = wz::ResourceManagement::Get<wz::Texture>(sprite.hTexture);

                wz::Rect bounds(0, 0, 1600.f, 900.f);
                vec2 size = { texture.GetWidth(), texture.GetHeight() };
                
                position.value.x = std::max(bounds.Left(), position.value.x);
                position.value.x = std::min(bounds.Right() - size.x, position.value.x);

                return;
            }
        }
    }
};

class BrickController : public wz::System
{
public:
    BrickController()
    {
        AddComponentType<Brick>();
        AddComponentType<Powerup>(component_flag_optional);

        Subscribe(wz::EventType::app_init);
        Subscribe(wz::EventType::app_update);
    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {

    }
};

class GameTracker : public wz::System
{
public:
    GameTracker()
    {
        AddComponentType<Time>();
        AddComponentType<Text>();

        Subscribe(wz::EventType::app_init);
        Subscribe(wz::EventType::app_update);
    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& time = *components.Get<Time>();
        auto& text = *components.Get<Text>();

        switch (e.GetEventType())
        {
            case wz::EventType::app_init:
            {
                text.hFont = wz::ResourceManagement::Load<wz::Font>("DemoGame/InputMono-Black.ttf");
                time.value = 0.f;
                return;
            }
            case wz::EventType::app_update:
            {
                time.value += ((const wz::AppUpdateEvent&)e).GetDeltaTime();
                text.value = std::to_string(time.value) + "s";
                return;
            }
        }
    }
};

class GuiSystem : public wz::System
{
public:
    GuiSystem()
    {
        AddComponentType<Gui>();

        Subscribe(wz::EventType::app_update);
    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& gui = *components.Get<Gui>();

        ImGui::BeginMainMenuBar();

        ImGui::Checkbox("Game Debugging", &gui.showGameDebugging);
        ImGui::Checkbox("Renderer2D Metrics", &gui.showMetrics);

        ImGui::EndMainMenuBar();

        if (gui.showGameDebugging)
        {
            ImGui::Begin("Game Debugging", &gui.showGameDebugging);
            ImGui::Checkbox("Draw Colliders", &g_drawColliders);
            ImGui::End();
        }

        if (gui.showMetrics)
        {
            auto& metrics = wz::Renderer2D::GetMetrics();
            ImGui::Begin("Renderer2D Metrics", &gui.showMetrics);
            ImGui::Text("%i Quads", metrics.numQuads);
            ImGui::Text("%i Indices", metrics.numIndices);
            ImGui::Text("%i vertices", metrics.GetNumVertices());
            ImGui::Text("%i Draw Calls", metrics.numDrawCalls);
            ImGui::Text("%i Memory usage (bytes)", metrics.GetMemoryUsage());
            ImGui::Text("%f Frame time (ms)", ((const wz::AppUpdateEvent&)e).GetDeltaTime());
            ImGui::End();
        }
}
};

class CollisionBoxUpdater : public wz::System
{
public:
    CollisionBoxUpdater()
    {
        AddComponentType<Position>();
        AddComponentType<CollisionBox>();
        AddComponentType<Sprite>();

        Subscribe(wz::EventType::app_update);
    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& position = *components.Get<Position>();
        auto& collisionBox = *components.Get<CollisionBox>();
        auto& sprite = *components.Get<Sprite>();

        switch (e.GetEventType())
        {
            case wz::EventType::app_update:
            {
                wz::Texture* texture;
                if (wz::ResourceManagement::TryGet<wz::Texture>(sprite.hTexture, texture))
                {
                    auto sz = vec2(texture->GetWidth(), texture->GetHeight());

                    collisionBox.rectangle.position = position.value;
                    collisionBox.rectangle.size = sz;
                }
                return;
            }
        }
    }
};

class BallController : public wz::System
{
public:
    BallController()
    {
        AddComponentType<Position>();
        AddComponentType<CollisionBox>();
        AddComponentType<Ball>();

        Subscribe(wz::EventType::app_init);
        Subscribe(wz::EventType::app_update);
    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& position = *components.Get<Position>();
        auto& collisionBox = *components.Get<CollisionBox>();
        auto& ball = *components.Get<Ball>();

        switch (e.GetEventType())
        {
            case wz::EventType::app_init:
            {
                ball.spawnPos = position.value;
                return;
            }
            case wz::EventType::app_update:
            {
                auto& delta = ((const wz::AppUpdateEvent&)e).GetDeltaTime();
                auto& wnd = wz::Application::Get().GetWindow();

                auto& ballBounds = collisionBox.rectangle;
                auto bounds = wz::Rect(0, 0, wnd.GetWidth(), wnd.GetHeight());

                // Touch side
                if (ballBounds.Right() >= bounds.Right() 
                    || ballBounds.Left() <= bounds.Left())
                {
                    ball.direction.x *= -1.f;
                }

                // Touch top
                if (ballBounds.Top() >= bounds.Top())
                {
                    ball.direction.y *= -1.f;
                }

                // Touch bottom
                if (ballBounds.Bottom() <= bounds.Bottom())
                {
                    position.value = ball.spawnPos;
                    ball.direction.y = (float)((rand() % 100) + 10) / 100.f;
                    ball.direction.x = (float)(rand() % 100) / 100.f;
                    ball.direction = glm::normalize(ball.direction);
                }

                position.value.x = std::max(bounds.Left(), position.value.x);
                position.value.x = std::min(bounds.Right() - ballBounds.w / 2.f, position.value.x);
                position.value.y = std::max(bounds.Bottom(), position.value.y);
                position.value.y = std::min(bounds.Top() - ballBounds.h / 2.f, position.value.y);

                g_ecs->ForEachEntity([&](wz::EntityHandle e) 
                {
                    if (e == position.entity) return true;

                    if (g_ecs->HasComponent<CollisionBox>(e))
                    {
                        auto otherBox = g_ecs->GetComponent<CollisionBox>(e);

                        wz::Rect intersection;
                        if (ballBounds.Intersects(otherBox->rectangle, &intersection))
                        {
                            if (intersection.w > intersection.h)
                            {
                                ball.direction.y *= -1;
                                if (g_ecs->HasComponent<PlayerConfig>(e))
                                {
                                    float ratio = (ballBounds.Center().x - otherBox->rectangle.Left()) / otherBox->rectangle.w;
                                    ball.direction.x = 1.f - 2.f * (1.f - ratio);
                                }
                                if (ball.direction.y > 0) // Top
                                {
                                    position.value.y = intersection.Top() + 1.f;
                                }
                                else // Bottom
                                {
                                    position.value.y = intersection.Bottom() - 1.f - ballBounds.h;
                                }
                            }
                            else 
                            {
                                ball.direction.x *= -1;
                                if (ball.direction.x > 0) // Left 
                                {
                                    position.value.x = intersection.Right() + 1.f;
                                }
                                else // Right
                                {
                                    position.value.x = intersection.Left() - 1.f - ballBounds.w;
                                }
                            }

                            if (g_ecs->HasComponent<Brick>(e))
                            {
                                g_ecs->RemoveEntity(e);
                            }

                            return false;
                        }
                    }

                    return true;
                });

                ball.direction = glm::normalize(ball.direction);

                position.value += ball.direction * ball.speed * delta;

                return;
            }
        }
    }
};

class ResourceSystem : public wz::System
{
public:
    ResourceSystem()
    {
        AddComponentType<ResourceComponent>();

        Subscribe(wz::EventType::app_update);
        Subscribe(wz::EventType::resource_file_change);
    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& comp = *components.Get<ResourceComponent>();
        switch(e.GetEventType())
        {
            case wz::EventType::app_update:
            {
                try
                {
                    wz::ResourceManagement::Validate();
                }
                catch(const wz::ResourceFileMissingException& e)
                {
                    // Ignore missing files for now
                    // (Photoshop deletes & replace files when exporting to it
                    //  and we don't want the resource to be removed because of that)
                }
                
                return;
            }
            case wz::EventType::resource_file_change:
            {
                auto& resourceEvent = (const wz::ResourceFileChangedEvent&)e;

                wz::ResourceManagement::Reload(resourceEvent.GetResourceHandle());

                return;
            }
        }
    }
};

class TestClass
{
public:


private:
};

class SpriteRenderer : public wz::System
{
public:
    SpriteRenderer()
    {
        AddComponentType<Position>();
        AddComponentType<Sprite>();
        AddComponentType<CollisionBox>(component_flag_optional);
        AddComponentType<ColorTint>(component_flag_optional);

        Subscribe(wz::EventType::app_render);
        Subscribe(wz::EventType::app_frame_end);

        
    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& position = *components.Get<Position>();
        auto& sprite = *components.Get<Sprite>();

        const wz::Color& color = components.Has<ColorTint>()
                                ? components.Get<ColorTint>()->value
                                : wz::Color::white;

        switch (e.GetEventType())
        {
            case wz::EventType::app_render:
            {
                if (wz::ResourceManagement::IsLoaded(sprite.hTexture))
                {       
                    wz::Renderer2D::SubmitTexture
                    (
                        g_mainPipeline,
                        sprite.hTexture,
                        position.value,
                        { 1.f, 1.f },
                        0, 
                        color
                    );
                }
                else 
                {
                    wz::Renderer2D::SubmitRect
                    (
                        g_mainPipeline,
                        wz::Rect(position.value, { 32, 32 }),
                        color
                    );
                }
                
                if (components.Has<CollisionBox>() && g_drawColliders)
                {
                    auto& collisionBox = *components.Get<CollisionBox>();

                    wz::Renderer2D::SubmitRect
                    (
                        g_mainPipeline,
                        collisionBox.rectangle,
                        wz::Color(.8f, .1f, .1f, .5f)
                    );
                }

                return;
            }
            case wz::EventType::app_frame_end:
            {
                
                return;
            }
        }
    }
};

class TextRenderer : public wz::System
{
public:
    TextRenderer()
    {
        AddComponentType<Position>();
        AddComponentType<Text>();

        Subscribe(wz::EventType::app_render);

    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& position = *components.Get<Position>();
        auto& text = *components.Get<Text>();

        const wz::Color& color = components.Has<ColorTint>()
                                ? components.Get<ColorTint>()->value
                                : wz::Color::white;

        switch (e.GetEventType())
        {
            case wz::EventType::app_render:
            {
                wz::Renderer2D::SubmitText
                (
                    g_textPipeline,
                    text.hFont,
                    text.value,
                    position.value,
                    { 1.f, 1.f },
                    0.f,
                    color
                );

                return;
            }
        }
    }
};

class CameraSystem : public wz::System
{
public:
    CameraSystem()
    {
        AddComponentType<Camera>();

        Subscribe(wz::EventType::app_init);
        Subscribe(wz::EventType::app_update);
        Subscribe(wz::EventType::app_frame_end);
        Subscribe(wz::EventType::window_resize);
    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& camera = *components.Get<Camera>();
        auto& wnd = wz::Application::Get().GetWindow();

        switch (e.GetEventType())
        {
            case wz::EventType::app_init:
            {
                wz::RenderCommand::SetClearColor(wz::Color::blue);
                camera.projection = glm::ortho<float>
                                    (
                                        0.f, 
                                        wnd.GetWidth(), 
                                        0.f, 
                                        wnd.GetHeight()
                                    );
                wz::RenderCommand::SetViewport(wz::Viewport
                                                (
                                                    0, 
                                                    0, 
                                                    wnd.GetWidth(), 
                                                    wnd.GetHeight()
                                                ));
                g_mainPipeline->camTransform = camera.projection;
                g_textPipeline->camTransform = camera.projection;
                return;
            }
            case wz::EventType::window_resize:
            {
                wz::RenderCommand::SetViewport(wz::Viewport
                                                (
                                                    0, 
                                                    0, 
                                                    wnd.GetWidth(), 
                                                    wnd.GetHeight()
                                                ));
                g_textPipeline->camTransform = camera.projection;
                return;
            }
            case wz::EventType::app_update:
            {
                wz::Renderer2D::ResetMetrics();

                wz::Renderer2D::Clear(g_mainPipeline);
                wz::Renderer2D::Begin(g_mainPipeline);

                wz::Renderer2D::Clear(g_textPipeline);
                wz::Renderer2D::Begin(g_textPipeline);

                g_lightCounter = 0;

                return;
            }
            case wz::EventType::app_frame_end:
            {
                wz::Renderer2D::End(g_mainPipeline);
                wz::Renderer2D::End(g_textPipeline);
                return;
            }
        }
    }
};

class LightSystem : public wz::System
{
public:
    LightSystem()
    {
        AddComponentType<Position>();
        AddComponentType<LightConfig>();
        AddComponentType<ColorTint>(component_flag_optional);

        Subscribe(wz::EventType::app_update);

    }

    void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const
    {
        auto& position = *components.Get<Position>();
        auto& lightConfig = *components.Get<LightConfig>();

        const wz::Color& color = components.Has<ColorTint>()
                                ? components.Get<ColorTint>()->value
                                : wz::Color::white;

        switch (e.GetEventType())
        {
            case wz::EventType::app_update:
            {
                position.value = wz::Input::GetMousePos();

                if (wz::ResourceManagement::IsLoaded(g_mainPipeline->hShader))
                {
                    auto& shader = wz::ResourceManagement::Get<wz::Shader>(g_mainPipeline->hShader);

                    shader.Upload1f("u_ambient", lightConfig.ambient);
                    shader.Upload2f("u_pointLights[0].lightPos", position.value);
                    shader.Upload4f("u_pointLights[0].lightColor", color);
                    shader.Upload1f("u_pointLights[0].intensity", lightConfig.intensity);
					shader.Upload1f("u_pointLights[0].radius", lightConfig.radius);

                    shader.Upload1f("u_ambient", lightConfig.ambient);
                    shader.Upload2f("u_pointLights[1].lightPos", position.value - vec2(0, 400));
                    shader.Upload4f("u_pointLights[1].lightColor", color);
                    shader.Upload1f("u_pointLights[1].intensity", lightConfig.intensity);
					shader.Upload1f("u_pointLights[1].radius", lightConfig.radius);

                    shader.Upload1i("u_pointLightCount", 2);
                }

				ImGui::Begin("Game Debugging");

				ImGui::SliderFloat("Ambient Strength", &lightConfig.ambient, 0.f, .9f);
				if (components.Has<ColorTint>())
					ImGui::ColorEdit4("Light color", components.Get<ColorTint>()->value.rgba);
				ImGui::DragFloat("Intensity", &lightConfig.intensity, .01f, .0f, 100.f);
				ImGui::DragFloat("Radius", &lightConfig.radius, .1f, .0f, 10000.f);

				ImGui::End();

                return;
            }
        }
    }
};

void DemoGame2D::Init()
{
    wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);

    string resDir = ulib::File::directory_of(GetExecutablePath()) + "/../../../res/";
    wz::ResourceManagement::SetResourceDir(resDir);

    g_mainPipeline = new wz::Renderer2D::Pipeline();
    g_mainPipeline->hShader 
                    = wz::ResourceManagement::Load<wz::Shader>("DemoGame/simple_light.wzsdr");
    g_textPipeline  = new wz::Renderer2D::Pipeline();
    g_textPipeline->hShader = wz::Renderer2D::GetTextShader();

    {
        auto e = m_ecs.CreateEntity<Position, Sprite>();
        m_ecs.GetComponent<Sprite>(e)->hTexture
                    = wz::ResourceManagement::Load<wz::Texture>("DemoGame/background.png");
    }

    {
        Position p;
        p.value = { 800, 100 };
        Sprite s;
        s.hTexture = wz::ResourceManagement::Load<wz::Texture>("DemoGame/player.png");
        PlayerConfig pc;
        CollisionBox cb;

        m_ecs.CreateEntity(p, s, pc, cb);
    }

    {
        m_ecs.CreateEntity<Camera>();
    }

    {
        m_ecs.CreateEntity<Gui>();
    }

    wz::Texture::Handle hBrickTexture
                    = wz::ResourceManagement::Load<wz::Texture>("DemoGame/brick.png");
    wz::Texture& brickTexture = wz::ResourceManagement::Get<wz::Texture>(hBrickTexture);

    {
        float countx = 20;
        float county = 10;
        float sizex = brickTexture.GetWidth();
        float sizey = brickTexture.GetHeight();

        float width = sizex * countx;
        float height = sizey * county;

        auto& wnd = wz::Application::Get().GetWindow();
        vec2 center = vec2(wnd.GetWidth(), wnd.GetHeight()) / 2.f;
        wz::Rect bounds(center.x - width / 2.f, wnd.GetHeight() - height - sizey * 2.f, width, height);

        int nMaxPowerups = 5;
        
        for (float x = bounds.Left(); x < bounds.Right(); x += sizex)
        {
            for (float y = bounds.Bottom(); y < bounds.Top(); y += sizey)
            {
                Position p;
                p.value = { x, y };
                Brick b;
                Sprite s;
                CollisionBox cb;
                s.hTexture = hBrickTexture;
                ColorTint ct;
                ct.value = wz::Color
                        (
                            (x + y) / (bounds.Right() + bounds.Top()), 
                            x / bounds.Right(), 
                            y / bounds.Top(), 
                            1.f
                        );

                auto e = m_ecs.CreateEntity(p, b, s, cb, ct);

                if (rand() % (int)(countx + county) == (int)countx)
                {
                    Powerup pu;
                    m_ecs.AddComponent(e, &pu);
                }
            }    
        }
    }

    {
        m_ecs.CreateEntity<ResourceComponent>();
    }

    {
        // Game Tracker Entity
        m_ecs.CreateEntity<Time, Text, Position>();
    }

    {
        Position p;
        p.value = {800, 150};
        Ball b;
        b.direction =  { 1.f, 1.f };
        CollisionBox cb;
        Sprite s;
        s.hTexture = wz::ResourceManagement::Load<wz::Texture>("DemoGame/ball.png");

        m_ecs.CreateEntity(p, b, cb, s);
    }

    {
        auto e = m_ecs.CreateEntity<LightConfig, Position, ColorTint>();
        m_ecs.GetComponent<ColorTint>(e)->value = wz::Color::white;
    }

    m_systems.AddSystem<BallController>();
    m_systems.AddSystem<PlayerController>();
    m_systems.AddSystem<GuiSystem>();
    m_systems.AddSystem<CameraSystem>();
    m_systems.AddSystem<CollisionBoxUpdater>();
    m_systems.AddSystem<SpriteRenderer>();
    m_systems.AddSystem<BrickController>();
    m_systems.AddSystem<ResourceSystem>();
    m_systems.AddSystem<GameTracker>();
    m_systems.AddSystem<TextRenderer>();
    m_systems.AddSystem<LightSystem>();

    g_ecs = &m_ecs;

    constexpr char code[] = R"(
        test_var = 5
    )";

    wz::ResourceManagement::CreateRuntime<wz::Script, const char[27]>(code);
}
void DemoGame2D::Shutdown()
{

}