#pragma once

#define _COMP_DECL(tname) struct tname : public wz::Component<tname>

_COMP_DECL(ResourceManagerComponent)
{
	ResourceManagerComponent()
	{
		string d = ulib::File::directory_of(GetExecutablePath()) + "/../../../res/";
		strcpy(resourceDir, d.c_str());
	}
	char resourceDir[1024];

	// ResourceManagement validations per seconds
	float validationRate = 1.f;

	float timeSinceValidate = 0.f;
};

_COMP_DECL(RendererManagerComponent)
{
	u32 cameraIndex = 0;
	
};

_COMP_DECL(CameraComponent)
{
public:
	float zoom = 0;
	mat4 transform = mat4(1.f);
	mat4 projection = mat4(1.f);
	wz::RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE;
	wz::Shader::Handle hShader = WZ_NULL_RESOURCE_HANDLE;
	bool rendererReady = false;
};

_COMP_DECL(TextureComponent)
{
	wz::Texture::Handle hTexture = WZ_NULL_RESOURCE_HANDLE;
	wz::Color color;
};

_COMP_DECL(TransformComponent)
{
	vec2 position = vec2(0.f);
	vec2 scale = vec2(1.f);
	float rotation = 0;
};

_COMP_DECL(LabelComponent)
{
	LabelComponent()
	{
		hndFont = 12;
	}
	char text[128] = "Hi, I am text!";
	wz::Font::Handle hndFont = 12;
};