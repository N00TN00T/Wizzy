#pragma once

#include <Wizzy.h>

#include <Wizzy/Extensions/Game.h> 

class DemoGame2D : public wz::Application
{
public:
    void Init()          override;
    void LoadResources() override;
    void Shutdown()      override;

    void GenerateBricks(const wz::Texture::Handle& hBrickTexture);
};