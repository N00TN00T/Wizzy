#pragma once

#include "Components.h"

class ResourceManager : public Wizzy::System
{
public:
	ResourceManager();

	virtual void OnEvent(const Wizzy::Event& e, Wizzy::ComponentGroup& components) const override;

};

class TextureRenderer : public Wizzy::System
{
public:
	TextureRenderer();

	virtual void OnEvent(const Wizzy::Event& e, Wizzy::ComponentGroup& components) const override;
};

class RendererManager : public Wizzy::System
{
public:
	RendererManager();

	virtual void OnEvent(const Wizzy::Event& e, Wizzy::ComponentGroup& components) const override;
};

class CameraSystem : public Wizzy::System
{
public:
	CameraSystem();

	virtual void OnEvent(const Wizzy::Event& e, Wizzy::ComponentGroup& components) const override;
};