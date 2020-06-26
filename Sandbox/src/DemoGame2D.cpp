#include "spch.h"

#include "DemoGame2D.h"

// Personal Preference
#define COMPONENT_DEF(n) struct n : public wz::Component<n>
#define SYSTEM_DEF(n, ...) class n : public wz::System<n, __VA_ARGS__>
// SYSTEM_DEF(MySystem, MyComponentA, MyComponentB ...)

namespace wzg = wz::Game;

u16 g_lightCounter = 0;
wz::EcsInstance* g_ecs;
bool g_drawColliders = false;
bool g_showCameras = false;

COMPONENT_DEF(PlayerConfig)
{   
    float speed = 500.f;
};

COMPONENT_DEF(Brick)
{
    bool brighter = true;
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
    wz::fvec2 direction;
    float speed = 650.f;
    wz::fvec2 spawnPos;
};

COMPONENT_DEF(Gui)
{
    bool showMetrics = false;
    bool showGameDebugging = false;
    std::vector<wz::EntityHandle> cameraEntities;
};

COMPONENT_DEF(Time)
{
    float value = 0.f;
};

COMPONENT_DEF(ResourceComponent)
{
    
};

COMPONENT_DEF(Name)
{
    string value = "Unnamed";
};

COMPONENT_DEF(CameraConfig)
{
    wz::fvec2 resolution;
    bool dynamic = false;
};

SYSTEM_DEF(PlayerController, PlayerConfig, wzg::Position2D, wzg::Sprite)
{
public:
    PlayerController()
    {
        Subscribe(wz::EventType::app_init);
        Subscribe(wz::EventType::app_update);
    }

    bool ProcessComponents( const wz::Event& e, 
                            PlayerConfig* playerConfig, 
                            wzg::Position2D* position, 
                            wzg::Sprite* sprite
                            ) const override
    {
        switch (e.GetEventType())
        {
            case wz::EventType::app_init:
            {
                break;
            }
            case wz::EventType::app_update:
            {
                auto& updateEvent = (const wz::AppUpdateEvent&)e;
                float delta = updateEvent.GetDeltaTime();

                if (wz::Input::GetKey(WZ_KEY_A))
                {   
                    position->value.x -= delta * playerConfig->speed;
                }
                if (wz::Input::GetKey(WZ_KEY_D))
                {
                    position->value.x += delta * playerConfig->speed;
                }

                auto& texture = wz::ResourceManagement::Get<wz::Texture>(sprite->hTexture);

                wz::Rect bounds(0, 0, 1600.f, 900.f);
                wz::fvec2 size = { texture.GetWidth(), texture.GetHeight() };
                
                position->value.x = std::max(bounds.Left(), position->value.x);
                position->value.x = std::min(bounds.Right() - size.x, position->value.x);

                break;
            }
        }

        return false;
    }
};

SYSTEM_DEF(BrickController, Brick, Powerup, wzg::Light)
{
public:
    BrickController()
    {
        Flag<Powerup>(flag_optional);
        Flag<wzg::Light>(flag_optional);

        Subscribe(wz::EventType::app_init);
        Subscribe(wz::EventType::app_update);
    }

    bool ProcessComponents(const wz::Event& e, 
                           Brick* brick, 
                           Powerup* powerup,
                           wzg::Light* light) const override
    {
        
        switch (e.GetEventType())
        {
            case wz::EventType::app_update:
            {
                auto& delta = ((const wz::AppUpdateEvent&)e).GetDeltaTime();
                if (light)
                {
                    if (brick->brighter)
                    {
                        light->intensity += delta;
                        light->radius += delta * 5.f;

                        if (light->intensity >= 1.5f)
                        {
                            brick->brighter = false;
                        }
                    }
                    else
                    {
                        light->intensity -= delta;
                        light->radius -= delta * 5.f;

                        if (light->intensity <= 0.5f)
                        {
                            brick->brighter = true;
                        }
                    }
                }
                break;
            }
        }

        return false;
    }
};

SYSTEM_DEF(GameTracker, Time, wzg::Text)
{
public:
    GameTracker()
    {
        Subscribe(wz::EventType::app_update);
    }

    bool ProcessComponents(const wz::Event& e, Time* time, wzg::Text* text) const override
    {
        switch (e.GetEventType())
        {
            case wz::EventType::app_update:
            {
                time->value += ((const wz::AppUpdateEvent&)e).GetDeltaTime();
                text->value = std::to_string(time->value) + "s";

                ImGui::BeginMainMenuBar();

                bool lightingEnabled = wz::Game::IsLightingEnabled();
                ImGui::Checkbox("Lighting", &lightingEnabled);
                wz::Game::SetLightingEnabled(lightingEnabled);

                ImGui::EndMainMenuBar();

                break;
            }
        }
        return false;
    }
};

SYSTEM_DEF(GuiSystem, Gui)
{
public:
    GuiSystem()
    {
        Subscribe(wz::EventType::app_update);
    }

    bool ProcessComponents(const wz::Event& e, Gui* gui) const override
    {
        ImGui::BeginMainMenuBar();

        ImGui::Checkbox("Game Debugging", &gui->showGameDebugging);
        ImGui::Checkbox("Renderer2D Metrics", &gui->showMetrics);
        ImGui::Checkbox("Show Cameras", &g_showCameras);

        ImGui::EndMainMenuBar();

        if (gui->showGameDebugging)
        {
            ImGui::Begin("Game Debugging", &gui->showGameDebugging);
            ImGui::Checkbox("Draw Colliders", &g_drawColliders);
            ImGui::End();
        }

        if (gui->showMetrics)
        {
            auto& metrics = wz::Renderer2D::GetMetrics();
            ImGui::Begin("Renderer2D Metrics", &gui->showMetrics);
            ImGui::Text("%i Quads", metrics.numQuads);
            ImGui::Text("%i Indices", metrics.numIndices);
            ImGui::Text("%i vertices", metrics.GetNumVertices());
            ImGui::Text("%i Draw Calls", metrics.numDrawCalls);
            ImGui::Text("%i Memory usage (bytes)", metrics.GetMemoryUsage());
            ImGui::Text("%f Frame time (ms)", ((const wz::AppUpdateEvent&)e).GetDeltaTime());
            ImGui::End();
            wz::Renderer2D::ResetMetrics();
        }

        return false;
    }
};

SYSTEM_DEF(CollisionBoxUpdater, wzg::Position2D, CollisionBox, wzg::Sprite)
{
public:
    CollisionBoxUpdater()
    {
        Subscribe(wz::EventType::app_update);
    }

    bool ProcessComponents( const wz::Event& e, 
                            wzg::Position2D* position, 
                            CollisionBox* collisionBox, 
                            wzg::Sprite* sprite
                            ) const override
    {
        switch (e.GetEventType())
        {
            case wz::EventType::app_update:
            {
                wz::Texture* texture;
                if (wz::ResourceManagement::TryGet<wz::Texture>(sprite->hTexture, texture))
                {
                    auto sz = wz::fvec2(texture->GetWidth(), texture->GetHeight());

                    collisionBox->rectangle.position = position->value;
                    collisionBox->rectangle.size = sz;
                }
                break;
            }
        }

        return false;
    }
};

SYSTEM_DEF(BallController, wzg::Position2D, CollisionBox, Ball, wzg::Light)
{
public:
    BallController()
    {
        Subscribe(wz::EventType::app_init);
        Subscribe(wz::EventType::app_update);
    }

    bool ProcessComponents(const wz::Event& e, 
                           wzg::Position2D* position, 
                           CollisionBox* collisionBox, 
                           Ball* ball,
                           wzg::Light* light
                           ) const override
    {
        switch (e.GetEventType())
        {
            case wz::EventType::app_init:
            {
                ball->spawnPos = position->value;
                break;
            }
            case wz::EventType::app_update:
            {
                auto& delta = ((const wz::AppUpdateEvent&)e).GetDeltaTime();
                auto& wnd = wz::Application::Get().GetWindow();

                auto& ballBounds = collisionBox->rectangle;
                auto bounds = wz::Rect(0, 0, wnd.GetWidth(), wnd.GetHeight());

                // Touch side
                if (ballBounds.Right() >= bounds.Right() 
                    || ballBounds.Left() <= bounds.Left())
                {
                    ball->direction.x *= -1.f;
                }

                // Touch top
                if (ballBounds.Top() >= bounds.Top())
                {
                    ball->direction.y *= -1.f;
                }

                // Touch bottom
                if (ballBounds.Bottom() <= bounds.Bottom())
                {
                    position->value = ball->spawnPos;
                    ball->direction.y = (float)((rand() % 100) + 10) / 100.f;
                    ball->direction.x = (float)(rand() % 100) / 100.f;
                    ball->direction = ball->direction.normalize();
                }

                position->value.x = std::max(bounds.Left(), position->value.x);
                position->value.x = std::min(bounds.Right() - ballBounds.w / 2.f, position->value.x);
                position->value.y = std::max(bounds.Bottom(), position->value.y);
                position->value.y = std::min(bounds.Top() - ballBounds.h / 2.f, position->value.y);

                // TODO: Do something less imbecile
                g_ecs->ForEachEntity([&](wz::EntityHandle e) 
                {
                    if (e == position->entity) return true;

                    if (g_ecs->HasComponent<CollisionBox>(e))
                    {
                        auto otherBox = g_ecs->GetComponent<CollisionBox>(e);

                        wz::Rect intersection;
                        if (ballBounds.Intersects(otherBox->rectangle, &intersection))
                        {
                            if (intersection.w > intersection.h)
                            {
                                ball->direction.y *= -1;
                                if (g_ecs->HasComponent<PlayerConfig>(e))
                                {
                                    float ratio = (ballBounds.Center().x - otherBox->rectangle.Left()) / otherBox->rectangle.w;
                                    ball->direction.x = 1.f - 2.f * (1.f - ratio);
                                }
                                if (ball->direction.y > 0) // Top
                                {
                                    position->value.y = intersection.Top() + 1.f;
                                }
                                else // Bottom
                                {
                                    position->value.y = intersection.Bottom() - 1.f - ballBounds.h;
                                }
                            }
                            else 
                            {
                                ball->direction.x *= -1;
                                if (ball->direction.x > 0) // Left 
                                {
                                    position->value.x = intersection.Right() + 1.f;
                                }
                                else // Right
                                {
                                    position->value.x = intersection.Left() - 1.f - ballBounds.w;
                                }
                            }

                            if (g_ecs->HasComponent<Brick>(e))
                            {
                                g_ecs->DestroyEntity(e);
                            }

                            return false;
                        }
                    }

                    return true;
                });

                ball->direction = ball->direction.normalize();

                position->value += ball->direction * ball->speed * delta;

                break;
            }
        }
        return false;
    }
};

SYSTEM_DEF(ResourceSystem, ResourceComponent)
{
public:
    ResourceSystem()
    {
        Subscribe(wz::EventType::app_update);
        Subscribe(wz::EventType::resource_file_change);
    }

    bool ProcessComponents(const wz::Event& e, ResourceComponent* resourceComponent) const override
    {
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
                    // TODO: Handle missing files.
                    // Ignore missing files for now
                    // (Photoshop deletes & replace files when exporting to it
                    //  and we don't want the resource to be removed because of that)
                }
                
                break;
            }
            case wz::EventType::resource_file_change:
            {
                auto& resourceEvent = (const wz::ResourceFileChangedEvent&)e;
                auto& hResource = resourceEvent.GetResourceHandle();

                wz::ResourceManagement::Reload(hResource);

                // TODO: Update pipeline shaders

                break;
            }
        }

        return false;
    }
};

SYSTEM_DEF(LightGuiSystem, wzg::Light, wzg::Position2D, Name, wzg::ColorTint)
{
public:
    LightGuiSystem()
    {
        Flag<wzg::ColorTint>(flag_optional);
        Flag<Name>(flag_optional);

        Subscribe(wz::EventType::app_update);
    }

    bool ProcessComponents(const wz::Event& e, 
                           wzg::Light* light,
                           wzg::Position2D* position,
                           Name*            name,
                           wzg::ColorTint* colorTint) const override
    {

        switch (e.GetEventType())
        {
            case wz::EventType::app_update:
            {
                string label = "Light " + (name 
                            ? name->value 
                            : "Unnamed##" + std::to_string(reinterpret_cast<int>(light)));
                ImGui::Begin(label.c_str());

                ImGui::DragFloat2("Position", position->value.ptr, .1f);
                ImGui::DragFloat("Intensity", &light->intensity, .01f);
                ImGui::DragFloat("Radius", &light->radius, .1f);
                if (colorTint)
                {
                    ImGui::ColorEdit4("Color", colorTint->value.ptr);
                }

                ImGui::End();
                break;
            }
        }
        return false;
    }
};

SYSTEM_DEF(CameraGuiSystem, wzg::Camera2D, wzg::Position2D, Name, CameraConfig)
{
public:
    CameraGuiSystem()
    {
        Subscribe(wz::EventType::app_update);
    }

    bool ProcessComponents(const wz::Event& e, 
                            wzg::Camera2D* camera,
                            wzg::Position2D* position,
                            Name* name,
                            CameraConfig* config
                            ) const override
    {
        switch(e.GetEventType())
        {
            case wz::EventType::app_update:
            {
                
                if (g_showCameras)
                {
                    DoGui(camera, position, name, config);
                }
                
                break;
            }
        }

        return false;
    }

    void DoGui(wzg::Camera2D* camera,
               wzg::Position2D* position,
               Name* name,
               CameraConfig* config
               ) const
    {
        if (wz::ResourceManagement::IsLoaded(camera->hRenderTarget))
        {
            auto& style = ImGui::GetStyle();
            auto& renderTarget 
                = wz::ResourceManagement::Get<wz::RenderTarget>(camera->hRenderTarget);

            s32 flags = ImGuiWindowFlags_NoScrollbar 
                        | ImGuiWindowFlags_NoScrollWithMouse
                        | ImGuiWindowFlags_MenuBar;
            ImGui::Begin(name->value.c_str(), &g_showCameras, flags);
            
            auto min = ImGui::GetWindowContentRegionMin();
            auto max = ImGui::GetWindowContentRegionMax();

            auto space = ImVec2(max.x - min.x, max.y - min.y);
            space.x += style.WindowPadding.x * 2;
            space.y += style.WindowPadding.y * 2;

            auto wndSz = ImGui::GetWindowSize();

            ImGui::BeginMenuBar();

            if (ImGui::BeginMenu("Config"))
            {
                string current = 
                        std::to_string(renderTarget.GetWidth()) 
                    + "x"
                    + std::to_string(renderTarget.GetHeight());

                if (config->dynamic)
                {
                    current = "Dynamic";
                }

                if (ImGui::BeginCombo("Resolution", current.c_str()))
                {
                    if (ImGui::MenuItem("1280x720"))
                    {
                        renderTarget.SetSize(1280, 720);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("1600x900"))
                    {
                        renderTarget.SetSize(1600, 900);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("1920x1080"))
                    {
                        renderTarget.SetSize(1920, 1080);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("2560x1440"))
                    {
                        renderTarget.SetSize(2560, 1440);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("3840x2160"))
                    {
                        renderTarget.SetSize(2840, 2160);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("7680x4320"))
                    {
                        renderTarget.SetSize(7680, 4320);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("160x90"))
                    {
                        renderTarget.SetSize(160, 90);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("80x45"))
                    {
                        renderTarget.SetSize(80, 45);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("40x22"))
                    {
                        renderTarget.SetSize(40, 22);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("1080x2340"))
                    {
                        renderTarget.SetSize(1080, 2340);
                        config->dynamic = false;
                    }
                    if (ImGui::MenuItem("Dynamic"))
                    {
                        config->dynamic = true;
                    }

                    ImGui::EndCombo();
                }

                ImGui::Checkbox("Consistent view Size", &camera->consistentSize);
                if (camera->consistentSize)
                {
                    ImGui::DragFloat2
                    (
                        "View Size", 
                        camera->viewSize.ptr, 
                        .1f, 
                        1.f, 
                        99999.f
                    );
                }
                ImGui::DragFloat2
                (
                    "Position", 
                    position->value.ptr,
                    .1f
                );
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();

            ImVec2 pos = { 0, wndSz.y - space.y };
            wz::fvec2 res = { renderTarget.GetWidth(), renderTarget.GetHeight() };

            ImVec2 sz = { res.x, res.y };

            if (config->dynamic)
            {
                if (renderTarget.GetWidth() != space.x
                    || renderTarget.GetWidth() != space.y)
                {
                    renderTarget.SetSize(std::max(space.x, 1.f), std::max(space.y, 1.f));
                    sz = { space.x, space.y };
                }
            }
            else
            {
                if (res.x > res.y)
                {
                    sz.x = space.x;
                    float ratio = res.y / res.x;
                    sz.y = sz.x * ratio;

                    if (space.y < sz.y)
                    {
                        sz.y = space.y;
                        float ratio = res.x / res.y;
                        sz.x = sz.y * ratio;
                        pos.x += (space.x - sz.x) / 2.f;
                    }
                    else 
                    {
                        pos.y += (space.y - sz.y) / 2.f;
                    }
                }
                else 
                {
                    sz.y = space.y;
                    float ratio = res.x / res.y;
                    sz.x = sz.y * ratio;

                    if (space.x < sz.x)
                    {
                        sz.x = space.x;
                        float ratio = res.y / res.x;
                        sz.y = sz.x * ratio;
                        pos.y += (space.y - sz.y) / 2.f;
                    }
                    else 
                    {
                        pos.x += (space.x - sz.x) / 2.f;
                    }
                }
            }

            ImGui::SetCursorPos(pos);

            ImGui::Image
            (
                (ImTextureID)renderTarget.GetTextureId(),
                sz,
                ImVec2(0, 1),
                ImVec2(1, 0)
            );
            ImGui::End();
        }
    }
};

void DemoGame2D::Init()
{
    // Set Log Levels. If Core Trace Level is activated the console will be hardcore spammed
    wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);
    wz::Log::SetClientLogLevel(LOG_LEVEL_TRACE);
}
void DemoGame2D::LoadResources()
{
    // Set Resource Directory (Portable)
    string resDir = ulib::File::directory_of(GetExecutablePath()) + "/../../../res/";
    wz::ResourceManagement::SetResourceDir(resDir);

    // Load Resource Files
    auto hBgrTex = wz::ResourceManagement::Load<wz::Texture>("DemoGame/background.png");
    auto hPlayerTexture = wz::ResourceManagement::Load<wz::Texture>("DemoGame/player.png");
    auto hBrickTexture = wz::ResourceManagement::Load<wz::Texture>("DemoGame/brick.png");
    auto hBallTexture = wz::ResourceManagement::Load<wz::Texture>("DemoGame/ball.png");
    auto hFont = wz::ResourceManagement::Load<wz::Font>("DemoGame/InputMono-Black.ttf");

    // Create the background
    auto hBackground = m_ecs.CreateEntity<wzg::Position2D, wzg::Sprite>();
    m_ecs.GetComponent<wzg::Sprite>(hBackground)->hTexture = hBgrTex;

    // Create the Player
    auto hPlayer = m_ecs.CreateEntity
    <
        wzg::Position2D, 
        wzg::Sprite, 
        PlayerConfig, 
        CollisionBox
    >();
    m_ecs.GetComponent<wzg::Position2D>(hPlayer)->value = { 800, 100 };
    m_ecs.GetComponent<wzg::Sprite>(hPlayer)->hTexture = hPlayerTexture;

    // Create the main camera
    wz::Game::CreateCamera(&m_ecs);

    // Create two cameras that render to their own render targets instead of the window
    auto hCamera1 = m_ecs.CreateEntity<wzg::Camera2D, wzg::Position2D, Name, CameraConfig>();
    auto hrt1 = wz::ResourceManagement::CreateRuntime<wz::RenderTarget>(1600, 900);
    m_ecs.GetComponent<wzg::Camera2D>(hCamera1)->hRenderTarget = hrt1;
    m_ecs.GetComponent<Name>(hCamera1)->value = "Camera 1";

    auto hCamera2 = m_ecs.CreateEntity<wzg::Camera2D, wzg::Position2D, Name, CameraConfig>();
    auto hrt2 = wz::ResourceManagement::CreateRuntime<wz::RenderTarget>(1600, 900);
    m_ecs.GetComponent<wzg::Camera2D>(hCamera2)->hRenderTarget = hrt2;
    m_ecs.GetComponent<Name>(hCamera2)->value = "Camera 2";

    // Create the Gui Entity
    m_ecs.CreateEntity<Gui>();

    // Generate the block of Bricks
    GenerateBricks(hBrickTexture);

    // Create the Resource Manager Entity
    m_ecs.CreateEntity<ResourceComponent>();

    // Create the Game Tracker Entity
    auto hGameTracker = m_ecs.CreateEntity<Time, wzg::Text, wzg::Position2D>();
    m_ecs.GetComponent<wzg::Text>(hGameTracker)->hFont = hFont;

    // Create the Ball
    auto hBall = m_ecs.CreateEntity
    <
        wzg::Position2D, 
        wzg::Sprite, 
        Ball, 
        CollisionBox, 
        wzg::Light, // Make the ball a light
        Name
    >();
    m_ecs.GetComponent<wzg::Position2D>(hBall)->value = { 800, 150 };
    m_ecs.GetComponent<wzg::Sprite>(hBall)->hTexture = hBallTexture;
    m_ecs.GetComponent<Ball>(hBall)->direction = { 1, 1 };
    m_ecs.GetComponent<Name>(hBall)->value = "Ball";

    wz::Game::SetLightingEnabled(false);

    // Register Systems to the Main System Layer
    // The main System Layer is notified of all events
    m_systemLayer.Push<BallController>();
    m_systemLayer.Push<PlayerController>();
    m_systemLayer.Push<GuiSystem>();
    m_systemLayer.Push<CollisionBoxUpdater>();
    m_systemLayer.Push<BrickController>();
    m_systemLayer.Push<ResourceSystem>();
    m_systemLayer.Push<GameTracker>();
    m_systemLayer.Push<CameraGuiSystem>();
    //m_systemLayer.Push<LightGuiSystem>();

    // Attach the Systems from the Game Library Extension to the Main System Layer
    wz::Game::Attach(m_systemLayer);

    g_ecs = &m_ecs;
}
void DemoGame2D::Shutdown()
{

}

void DemoGame2D::GenerateBricks(const wz::Texture::Handle& hBrickTexture)
{
    auto& brickTexture = wz::ResourceManagement::Get<wz::Texture>(hBrickTexture);

    float countx = 20;
    float county = 10;
    float sizex = brickTexture.GetWidth();
    float sizey = brickTexture.GetHeight();

    float width = sizex * countx;
    float height = sizey * county;

    auto& wnd = wz::Application::Get().GetWindow();
    wz::fvec2 center = wz::fvec2(wnd.GetWidth(), wnd.GetHeight()) / 2.f;
    wz::Rect bounds(center.x - width / 2.f, wnd.GetHeight() - height - sizey * 2.f, width, height);

    int nMaxPowerups = 5;
    int i = 0;
    for (float x = bounds.Left(); x < bounds.Right(); x += sizex)
    {
        for (float y = bounds.Bottom(); y < bounds.Top(); y += sizey)
        {
            wz::color color = wz::color
            (
                (x + y) / (bounds.Right() + bounds.Top()), 
                x / bounds.Right(), 
                y / bounds.Top(), 
                1.f
            );

            auto hPlatform = m_ecs.CreateEntity
            <
                wzg::Position2D,
                wzg::Sprite,
                wzg::ColorTint,
                Brick,
                CollisionBox,
                Name
            >();
            m_ecs.GetComponent<wzg::Position2D>(hPlatform)->value = { x, y };
            m_ecs.GetComponent<wzg::Sprite>(hPlatform)->hTexture = hBrickTexture;
            m_ecs.GetComponent<wzg::ColorTint>(hPlatform)->value = color;
            m_ecs.GetComponent<Name>(hPlatform)->value = "Brick " + std::to_string(i);
            

            if (rand() % ((int)(countx + county) / 2) == 1)
            {
                m_ecs.CreateComponent<Powerup>(hPlatform);
                m_ecs.CreateComponent<wzg::Light>(hPlatform);
                m_ecs.CreateComponent<wzg::LightOffset>(hPlatform);
                float lightStren = ((rand() % 100) / 100.f);
                m_ecs.GetComponent<wzg::Light>(hPlatform)->radius = 8.f + 16.f * lightStren;
                m_ecs.GetComponent<wzg::Light>(hPlatform)->intensity = .5f + lightStren;
                m_ecs.GetComponent<wzg::LightOffset>(hPlatform)->value = wz::fvec2(32, 16);
            }
            i++;
        }    
    }
}