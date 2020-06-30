#pragma once

#include "Wizzy.h"

#include "Wizzy/Extensions/ExtensionsCommon.h"

WZ_NS_BEGIN(Wizzy);
WZ_NS_BEGIN(Game);
    __COMPONENT_DECL(Position2D)
    {
        fvec2 value = fvec2(0);
    };

    __COMPONENT_DECL(Scale2D)
    {
        fvec2 value = fvec2(1);
    };

    __COMPONENT_DECL(Rotation2D)
    {
        float value = 0;
    };

    __COMPONENT_DECL(ColorTint)
    {
        color value;
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