

#pragma once

#include "Wizzy.h"

#define __COMPONENT_DECL(t) struct t : public Component<t>
#define __SYSTEM_DECL(t, ...) class t : public System<t, __VA_ARGS__>


WZ_NS_BEGIN(Wizzy);
WZ_NS_BEGIN(Game);
    __COMPONENT_DECL(Position2D)
    {
        vec2 value = vec2(0);
    };

    __COMPONENT_DECL(Scale2D)
    {
        vec2 value = vec2(1);
    };

    __COMPONENT_DECL(Rotation2D)
    {
        float value = 0;
    };

    __COMPONENT_DECL(ColorTint)
    {
        Color value;
    };

    __COMPONENT_DECL(ResourceConfig)
    {
        string resDir;
    };

    __COMPONENT_DECL(Text)
    {
        string value;
        Font::Handle hFont;
    };

    __SYSTEM_DECL(ResourceSystem, ResourceConfig)
    {   
    public:
        ResourceSystem();

        bool ProcessComponents(const Event& e, ResourceConfig* resourceConfig) const override;
    };


WZ_NS_END;
WZ_NS_END;