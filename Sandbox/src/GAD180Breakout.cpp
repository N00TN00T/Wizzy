#include "spch.h"

#include "GAD180Breakout.h"

namespace wzg = ::wz::Game;

void GAD180Breakout::Init() {
    wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);
    
}
void GAD180Breakout::LoadResources() {
    // Attach Game extension systems to main system layer
    wz::Game::Attach(m_systemLayer);
    // Attach Physics2D extension systems to main system layer
    // TODO...

    wzg::CreateCamera(&m_ecs);

    auto player = wzg::CreateSprite(&m_ecs, wz::fvec2(800 - 32, 450 - 32));

    m_ecs.CreateComponent<wzg::CharacterController2DInputMapping>(player);
    m_ecs.CreateComponent<wzg::CharacterController2DConfig>(player);
    m_ecs.CreateComponent<wzg::Scale2D>(player);

    auto mapping = m_ecs.GetComponent<wzg::CharacterController2DInputMapping>(player);
    
    mapping->upKey    = WZ_KEY_W;
    mapping->rightKey = WZ_KEY_D;
    mapping->downKey  = WZ_KEY_S;
    mapping->leftKey  = WZ_KEY_A;

    m_ecs.GetComponent<wzg::Sprite>(player)->hTexture = wz::Renderer2D::GetWhite1x1Texture();
    m_ecs.GetComponent<wzg::Scale2D>(player)->value = wz::fvec2(64, 64);
}
void GAD180Breakout::Shutdown() {

}