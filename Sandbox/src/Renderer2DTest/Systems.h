#pragma once

#include "Components.h"

class ResourceManager : public ecs::System
{
public:
	ResourceManager();

	virtual void OnEvent(const Wizzy::Event& e, ecs::ComponentGroup& components) const override;

};

class TextureRenderer : public ecs::System
{
public:
	TextureRenderer();

	virtual void OnEvent(const Wizzy::Event& e, ecs::ComponentGroup& components) const override;
};

class RendererManager : public ecs::System
{
public:
	RendererManager();

	virtual void OnEvent(const Wizzy::Event& e, ecs::ComponentGroup& components) const override;
};

class CameraSystem : public ecs::System
{
public:
	CameraSystem();

	virtual void OnEvent(const Wizzy::Event& e, ecs::ComponentGroup& components) const override;
};