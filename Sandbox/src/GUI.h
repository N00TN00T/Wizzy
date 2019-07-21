#pragma once

#include <Wizzy.h>

#include "Components/Common.h"
#include "Components/Rendering.h"

class GUISystem
    : public ecs::System {
public:
    static bool _dock;

    GUISystem();

    void DoGenericGUI(ecs::ComponentGroup& components) const;

    void RenderGameView(ViewComponent& view) const;

    virtual void OnEvent(const wz::Event& e,
                     ecs::ComponentGroup& components) const override;
};
