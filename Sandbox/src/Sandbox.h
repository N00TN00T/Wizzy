#pragma once

#include <Wizzy.h>
#include <Wizzy/PropertyLibrary.h>
#include <Wizzy/Instrumentor.h>

struct EngineData : public ecs::Component<EngineData>
{

	string resourcePath;
	float validationRate = 1.f;
	float timeSinceValidate = 0.f;
	wz::Shader::Handle hndShader = WZ_NULL_RESOURCE_HANDLE;
	wz::Texture::Handle hndTexture = WZ_NULL_RESOURCE_HANDLE;
	std::shared_ptr<wz::IWindow> window = NULL;
	vec2 pos = vec2(0);
	vec2 scale = vec2(1);
	float rotation = 0;
	string ftStr = "???";
	wz::RenderTargetPtr renderTarget = NULL;
};

class EngineManagerSystem : public ecs::System
{
public:
	EngineManagerSystem();

	virtual void OnEvent(const wz::Event& e, ecs::ComponentGroup& components) const override;
};

class Sandbox
	: public wz::Application
{
public:

	Sandbox();

	~Sandbox();

	virtual void Init() override;
	
	virtual void Shutdown() override;

};