#pragma once

#include "Wizzy.h"

#include "Wizzy/Extensions/Game/Common.h"
#include "Wizzy/Extensions/ExtensionsCommon.h"

WZ_NS_BEGIN(Wizzy);
WZ_NS_BEGIN(Game);

enum class CharacterController2DMode {
    TopDown, Platformer
};

__COMPONENT_DECL(CharacterController2DInputMapping) {
    s32 upKey, rightKey, downKey, leftKey;
};

__COMPONENT_DECL(CharacterController2DConfig) {
    CharacterController2DMode mode = CharacterController2DMode::TopDown;
    f32 baseSpeed = 1000;
};

__SYSTEM_DECL(CharacterController2D, 
              CharacterController2DInputMapping, 
              CharacterController2DConfig, 
              Position2D)
{
public:
    CharacterController2D();

    bool ProcessComponents(const Event& e, 
                           CharacterController2DInputMapping* controllerInputMapping,
                           CharacterController2DConfig* controllerConfig,
                           Position2D* position
                           ) const override;
};

WZ_NS_END;
WZ_NS_END;