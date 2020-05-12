#pragma once

#include <Wizzy.h>
#include <Wizzy/Extensions/Game.h>

class Sandbox
	: public wz::Application
{
public:

	Sandbox();

	~Sandbox();

private:
	virtual void Init() override;
	
	virtual void Shutdown() override;

	void OnEvent(wz::Event& e);

	bool Update(wz::AppUpdateEvent& e);

	bool Render(wz::AppRenderEvent& e);

private:
	wz::Texture::Handle texture;
	wz::Font::Handle font;
	wz::AudioClip::Handle song;
	wz::Renderer2D::Pipeline* pipeline;
	wz::Renderer2D::Pipeline* textPipeline;
	float frameTime = 0;
	bool textured = false;
	bool text = true;
	float sizex = 30.f;
	float sizey = 30.f;
	bool useGameExtension = false;
};