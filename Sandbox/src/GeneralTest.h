#pragma once

#include <Wizzy.h>

class GeneralTest : public wz::Application
{
public:
    void Init()          override;
    void LoadResources() override;
    void Shutdown()      override;
};