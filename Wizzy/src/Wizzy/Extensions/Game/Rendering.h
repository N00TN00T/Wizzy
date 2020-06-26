#pragma once

#include "Wizzy.h"

#include "Wizzy/Extensions/Game/Common.h"
#include "Wizzy/Extensions/ExtensionsCommon.h"
    
WZ_NS_BEGIN(Wizzy);
WZ_NS_BEGIN(Game);

const Shader::Handle& Get2DLightShader();

void SetLightingEnabled(bool enabled);
bool IsLightingEnabled();

typedef std::unordered_map
<
    Shader::Handle, 
    std::unordered_set<RenderPipeline2D*>,
    Shader::Handle::hash
> PipelinePool2D;

const PipelinePool2D& GetPipelinePool2D();

__COMPONENT_DECL(CustomShader)
{
    Shader::Handle hShader = WZ_NULL_RESOURCE_HANDLE;
};

__COMPONENT_DECL(Camera2D)
{
    RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE;
    std::unordered_map<Shader::Handle, RenderPipeline2D*, Shader::Handle::hash> pipelines;
    fvec2 viewSize = fvec2(1600, 900);
    bool upToDate = false;
    bool consistentSize = false;
    fmat4 transform;
};

__COMPONENT_DECL(Sprite)
{
    Texture::Handle hTexture = WZ_NULL_RESOURCE_HANDLE;
};

__COMPONENT_DECL(Light)
{
    float intensity = 1.f;
	float radius = 150.f;
};
__COMPONENT_DECL(LightOffset)
{
    fvec2 value = fvec2(0);
};

__SYSTEM_DECL(SpriteRenderer, Sprite, Position2D, ColorTint, Scale2D, Rotation2D, CustomShader)
{
public:
    SpriteRenderer();

    bool ProcessComponents(const Event& e, 
                           Sprite* sprite,
                           Position2D* position,
                           ColorTint* colorTint,
                           Scale2D* scale,
                           Rotation2D* rotation,
                           CustomShader* customShader
                           ) const override;
};

__SYSTEM_DECL(CameraSystem2D, Camera2D, Position2D, Rotation2D, Scale2D, ColorTint)
{
public:
    CameraSystem2D();

    bool ProcessComponents(const Event& e, 
                           Camera2D* camera,
                           Position2D* position,
                           Rotation2D* rotation,
                           Scale2D* scale,
                           ColorTint* colorTint
                           ) const override;

    void RegisterShader(Camera2D* camera, const Shader::Handle& hShader) const;
    void UnregisterShader(Camera2D* camera, const Shader::Handle& hShader) const;
    void UpdateSize(Camera2D* camera) const;
    void UpdateTransform(Camera2D* camera, 
                         Position2D* position, 
                         Scale2D* scale, 
                         Rotation2D* rotation
                         ) const;
};

__SYSTEM_DECL(TextRenderer, Text, Position2D, Rotation2D, Scale2D, ColorTint, CustomShader)
{
public:
    TextRenderer();

    bool ProcessComponents(const Event& e, 
                           Text* text,
                           Position2D* position,
                           Rotation2D* rotation,
                           Scale2D* scale,
                           ColorTint* colorTint,
                           CustomShader* customShader
                           ) const override;
};

__SYSTEM_DECL(LightRenderer2D, Position2D, Light, ColorTint, CustomShader, LightOffset)
{
public:
    LightRenderer2D();

    bool ProcessComponents(const Event& e,
                           Position2D* position,
                           Light* light,
                           ColorTint* color,
                           CustomShader* customShader,
                           LightOffset* offset
                           ) const override;
};

WZ_NS_END;
WZ_NS_END;