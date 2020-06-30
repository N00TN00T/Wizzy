#pragma once

// Wizzy Core
#include <Wizzy.h>

// Wizzy Extensions
#include <Wizzy/Extensions/Game.h>
#include <Wizzy/Extensions/Physics2D.h>

class GAD180Breakout : public wz::Application
{
public:
    void Init()          override;
    void LoadResources() override;
    void Shutdown()      override;
};