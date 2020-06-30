#pragma once

#ifdef WZ_EXTENSIONS_ENABLE_ALL || defined(WZ_EXTENSIONS_ENABLE_GAME)

#include "Game/Common.h"
#include "Game/Rendering.h"
#include "Game/CharacterControllers.h"

WZ_NS_BEGIN(Wizzy)
WZ_NS_BEGIN(Game)

inline void Attach(SystemLayer& layer)
{
    layer.Push<CharacterController2D>();
    layer.Push<CameraSystem2D>();
    layer.Push<SpriteRenderer>();
    layer.Push<TextRenderer>();
    layer.Push<LightRenderer2D>();
    layer.Push<ResourceSystem>();
}

inline EntityHandle CreateCamera(EcsInstance* ecs, const fvec2& position = fvec2(0))
{
    auto hEntity = ecs->CreateEntity<Camera2D, Position2D>();
    ecs->GetComponent<Position2D>(hEntity)->value = position;
    return hEntity;
}

inline EntityHandle CreateSprite(EcsInstance* ecs, const fvec2& position = fvec2(0))
{
    auto hEntity = ecs->CreateEntity<Sprite, Position2D>();
    ecs->GetComponent<Position2D>(hEntity)->value = position;
    return hEntity;
}

inline EntityHandle CreateLight(EcsInstance* ecs, 
                                const fvec2& position = fvec2(0), 
                                const color& color = COLOR_WHITE)
{
    auto hEntity = ecs->CreateEntity<Light, Position2D, ColorTint>();
    ecs->GetComponent<Position2D>(hEntity)->value = position;
    ecs->GetComponent<ColorTint>(hEntity)->value = color;
    return hEntity;
}

WZ_NS_END;
WZ_NS_END;

#else
    #error WZ_EXTENSIONS_ENABLE_GAME must be defined to use the Wizzy Game Extension
#endif